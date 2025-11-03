#include "gf3d_mesh.h"
#include "gf3d_camera.h"

void gf3d_mesh_draw(Mesh *mesh, GFC_Matrix4 modelMat, GFC_Color mod, Texture *texture, GFC_Vector3D lightPos, GFC_Color lightColor)
{
    MeshUBO ubo = { 0 };

    if (!mesh)return;
    gfc_matrix4_copy(ubo.model, modelMat);
    gf3d_vgraphics_get_view(&ubo.view);
    gf3d_vgraphics_get_projection_matrix(&ubo.proj);

    ubo.color = gfc_color_to_vector4f(mod);
    ubo.lightColor = gfc_color_to_vector4f(lightColor);
    ubo.lightPos = gfc_vector3dw(lightPos, 1.0);
    ubo.camera = gfc_vector3dw(gf3d_camera_get_position(), 1.0);
    gf3d_mesh_queue_render(mesh, mesh_manager.pipe, &ubo, texture);
}

void gf3d_mesh_queue_render(Mesh *mesh, Pipeline *pipe, void *uboData, Texture *texture)
{
    int i, c;
    MeshPrimitive* prim;
    if ((!mesh)||(!pipe)||(!uboData))return;
    c = gfc_list_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        prim = gfc_list_nth(mesh->primitives, i);
        if (!prim)continue;
        gf3d_mesh_primitive_queue_render(*prim, pipe, uboData, texture);
    }
}

void gf3d_mesh_primitive_queue_render(MeshPrimitive *prim, Pipeline *pipe, void *uboData, Texture *texture)
{
    if ((!prim)||(!pipe)||(!uboData))return;
    if (!texture)texture = mesh_manager.defaultTexture;
    gf3d_pipeline_queue_render(
        pipe,
        prim->vertexBuffer,
        prim->vertexCount,
        prim->faceBuffer,
        uboData,
        texture);
}

VkVertexInputBindingDescription * gf3d_mesh_manager_get_bind_description()
{
    mesh_manager.bindingDescription.binding = 0;
    mesh_manager.bindingDescription.stride = sizeof(Vertex);
    mesh_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return &mesh_manager.bindingDescription;
}

VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{
    mesh_manager.attributeDescriptions[0].binding = 0;
    mesh_manager.attributeDescriptions[0].location = 0;
    mesh_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    mesh_manager.attributeDescriptions[1].binding = 0;
    mesh_manager.attributeDescriptions[1].location = 1;
    mesh_manager.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    mesh_manager.attributeDescriptions[1].offset = offsetof(Vertex, normal);

    mesh_manager.attributeDescriptions[2].binding = 0;
    mesh_manager.attributeDescriptions[2].location = 2;
    mesh_manager.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    mesh_manager.attributeDescriptions[2].offset = offsetof(Vertex, texel);
    if (count)*count = MESH_ATTRIBUTE_COUNT;
    return mesh_manager.attributeDescriptions;
}

void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;
    if (mesh_max == 0)
    {
        slog("cannot intilizat sprite manager for 0 sprites");
        return;
    }
    mesh_manager.chain_length = gf3d_swapchain_get_chain_length();
    mesh_manager.mesh_list = (Mesh*)gfc_allocate_array(sizeof(Mesh), mesh_max);
    mesh_manager.mesh_count = mesh_max;
    mesh_manager.device = gf3d_vgraphics_get_default_logical_device();

    gf3d_mesh_get_attribute_descriptions(&count);
    mesh_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_manager_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );
    mesh_manager.defaultTexture = gf3d_texture_load("images/default.png");
    if (__DEBUG)slog("mesh manager initiliazed");
    atexit(gf3d_mesh_manager_close);
}

void gf3d_mesh_primitive_create_vertex_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vertex *vertices;
    Uint32 vcount;
    size_t bufferSize;
    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

    if (!prim)
    {
        slog("no mesh primitive provided");
        return;
    }

    vertices = prim->objData->faceVertices;
    vcount = prim->objData->face_vert_count;
    bufferSize = sizeof(Vertex) *vcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->vertexBuffer, &prim->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

    prim->vertexCount = vcount;

}

void gf3d_mesh_setup_face_buffers(MeshPrimitive *prim)
{
    void *data = NULL;
    Face *faces = NULL;
    Uint32 fcount;

    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkDeviceSize bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    if ((!prim)||(!prim->objData))return;
    faces = prim->objData->outFace;
    fcount = prim->objData->face_count;
    if ((!faces)||(!fcount))return;
    bufferSize = sizeof(Face) *fcount;

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &prim->faceBuffer, &prim->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, prim->faceBuffer, bufferSize);

    prim->faceCount = fcount;
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 worldPosition;
layout(location = 4) in vec4 cameraPos;
layout(location = 5) in vec4 lightPos;
layout(location = 6) in vec4 lightColor;

layout(location = 0) out vec4 outColor;


void main()
{
    vec3 lightDir = normalize(lightPos.xyz - worldPosition.xyz);
    vec4 texColor = texture(texSampler, fragTexCoord);

    texColor.xyz = texColor.xyz * max(0.0, dot(-lightDir, inNormal));

    outColor = texColor * colorMod;
}

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
    mat4    model;
    mat4    view;
    mat4    proj;
    vec4    color;
    vec4    camera;
    vec4    lightPos;
    vec4    lightColor;
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 colorMod;
layout(location = 3) out vec4 worldPosition;
layout(location = 4) out vec4 cameraPos;
layout(location = 5) out vec4 lightPos;
layout(location = 6) out vec4 lightColor;

void main()
{
    mat3 normalMatrix;
    mat4 mvp = ubo.proj * ubo.view * ubo.model;

    //positions
    gl_Position = mvp * vec4(inPosition, 1.0);
    worldPosition = ubo.model * vec4(inPosition, 1.0);

    //normals
    normalMatrix = transpose(inverse(mat3(ubo.model)));
    outNormal = normalize(normalMatrix * inNormal);

    //pass throughs
    colorMod = ubo.color;
    cameraPos = ubo.camera;
    fragTexCoord = inTexCoord;
    lightPos = ubo.lightPos;
    lightColor = ubo.lightColor;
}
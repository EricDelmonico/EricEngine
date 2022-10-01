#include "Mesh.h"

//#include "Vertex.h"
//#include <DirectXMath.h>
//
//using DirectX::XMFLOAT3;
//
//Mesh::Mesh(std::shared_ptr<D3DResources> d3dResources, Microsoft::WRL::ComPtr<ID3D11Buffer> vb, Microsoft::WRL::ComPtr<ID3D11Buffer> ib, int indices) :
//    m_d3dResources(d3dResources), vertexBuffer(vb), indexBuffer(ib), indices(indices)
//{
//}
//
//Mesh Mesh::CreateCube(std::shared_ptr<D3DResources> d3dResources)
//{
//    // Things necessary for cube creation
//    Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
//    Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
//    int indices;
//
//    HRESULT hr = S_OK;
//
//    // Get our device
//    auto device = d3dResources->GetDevice();
//
//    // Create geometry
//    Vertex cubeVertices[] =
//    {
//        {XMFLOAT3(-0.5f,-0.5f,-0.5f), XMFLOAT3(  0,  0,  0)},
//        {XMFLOAT3(-0.5f,-0.5f, 0.5f), XMFLOAT3(  0,  0,  1)},
//        {XMFLOAT3(-0.5f, 0.5f,-0.5f), XMFLOAT3(  0,  1,  0)},
//        {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(  0,  1,  1)},
//        
//        {XMFLOAT3( 0.5f,-0.5f,-0.5f), XMFLOAT3(  1,  0,  0)},
//        {XMFLOAT3( 0.5f,-0.5f, 0.5f), XMFLOAT3(  1,  0,  1)},
//        {XMFLOAT3( 0.5f, 0.5f,-0.5f), XMFLOAT3(  1,  1,  0)},
//        {XMFLOAT3( 0.5f, 0.5f, 0.5f), XMFLOAT3(  1,  1,  1)},
//    };
//
//    // Create vertex buffer
//    CD3D11_BUFFER_DESC vDesc(
//        sizeof(cubeVertices),
//        D3D11_BIND_VERTEX_BUFFER);
//
//    D3D11_SUBRESOURCE_DATA vData = {};
//    vData.pSysMem = cubeVertices;
//    vData.SysMemPitch = 0;
//    vData.SysMemSlicePitch = 0;
//
//    hr = device->CreateBuffer(&vDesc, &vData, &vb);
//
//    // Create index buffer
//    unsigned int cubeIndices[] =
//    {
//        0,2,1, // -x
//        1,2,3,
//
//        4,5,6, // +x
//        5,7,6,
//
//        0,1,5, // -y
//        0,5,4,
//
//        2,6,7, // +y
//        2,7,3,
//
//        0,4,6, // -z
//        0,6,2,
//
//        1,3,7, // +z
//        1,7,5
//    };
//
//    indices = ARRAYSIZE(cubeIndices);
//
//    CD3D11_BUFFER_DESC iDesc(
//        sizeof(cubeIndices),
//        D3D11_BIND_INDEX_BUFFER);
//
//    D3D11_SUBRESOURCE_DATA iData = {};
//    iData.pSysMem = cubeIndices;
//    iData.SysMemPitch = 0;
//    iData.SysMemSlicePitch = 0;
//
//    hr = device->CreateBuffer(&iDesc, &iData, &ib);
//
//    return Mesh(d3dResources, vb, ib, indices);
//}

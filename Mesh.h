#pragma once

#include "DXCore.h"
#include<d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include"Vertex.h"
#include<vector>

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;                   //vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;                   //index buffer
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;      
	int indexCount;

	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	
	int GetIndexCount();
	
	void Draw();
	
	void CreateBuffers(Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices,
		Microsoft::WRL::ComPtr<ID3D11Device> device);

	Mesh(Vertex* vertexArray, int numVertices, unsigned int* indexArray, int numIndices, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);

	Mesh(const wchar_t* name, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);

	~Mesh();


};

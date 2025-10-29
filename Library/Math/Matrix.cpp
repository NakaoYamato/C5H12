#include "Matrix.h"

Matrix4X4 Matrix4X4::operator=(const Matrix4X4& m)
{
	this->m[0][0] = m.m[0][0];
	this->m[0][1] = m.m[0][1];
	this->m[0][2] = m.m[0][2];
	this->m[0][3] = m.m[0][3];
	this->m[1][0] = m.m[1][0];
	this->m[1][1] = m.m[1][1];
	this->m[1][2] = m.m[1][2];
	this->m[1][3] = m.m[1][3];
	this->m[2][0] = m.m[2][0];
	this->m[2][1] = m.m[2][1];
	this->m[2][2] = m.m[2][2];
	this->m[2][3] = m.m[2][3];
	this->m[3][0] = m.m[3][0];
	this->m[3][1] = m.m[3][1];
	this->m[3][2] = m.m[3][2];
	this->m[3][3] = m.m[3][3];
	return *this;
}

Matrix4X4 Matrix4X4::operator*(const Matrix4X4& m) const
{
	DirectX::XMMATRIX mat1 = DirectX::XMLoadFloat4x4(this);
	DirectX::XMMATRIX mat2 = DirectX::XMLoadFloat4x4(&m);
	DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(mat1, mat2);
	Matrix4X4 ret;
	DirectX::XMStoreFloat4x4(&ret, result);
	return ret;
}

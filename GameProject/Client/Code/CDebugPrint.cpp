#include "pch.h"
#include "CDebugPrint.h"
#include "Engine_Typedef.h"

std::ostream& operator<<(std::ostream& os, const Engine::_vec3& v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

std::ostream& operator<<(std::ostream& os, const D3DXMATRIX& mat)
{
    return os
        << mat._11 << ' ' << mat._12 << ' ' << mat._13 << ' ' << mat._14 << '\n'
        << mat._21 << ' ' << mat._22 << ' ' << mat._23 << ' ' << mat._24 << '\n'
        << mat._31 << ' ' << mat._32 << ' ' << mat._33 << ' ' << mat._34 << '\n'
        << mat._41 << ' ' << mat._42 << ' ' << mat._43 << ' ' << mat._44 << '\n';
}
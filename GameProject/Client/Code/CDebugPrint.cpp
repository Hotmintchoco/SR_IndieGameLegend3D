#include "pch.h"
#include "CDebugPrint.h"
#include "Engine_Typedef.h"

std::ostream& operator<<(std::ostream& os, const Engine::_vec3& v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")" << "\n";
}
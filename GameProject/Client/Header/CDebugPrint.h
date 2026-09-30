#pragma once

std::ostream& operator<<(std::ostream& os, const D3DXVECTOR3& v);

#define PRINT_INFO(x)							\
_vec3 vPos;										\
m_pTransformCom->Get_Info(x, &vPos);			\
cout << vPos << endl;							\
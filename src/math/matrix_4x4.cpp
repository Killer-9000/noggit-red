// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <math/matrix_4x4.hpp>

#include <cmath>
#include <cstring> // memcpy, memset
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace math
{
  matrix_4x4::uninitialized_t matrix_4x4::uninitialized;
  matrix_4x4::zero_t matrix_4x4::zero;
  matrix_4x4::unit_t matrix_4x4::unit;
  matrix_4x4::translation_t matrix_4x4::translation;
  matrix_4x4::scale_t matrix_4x4::scale;
  matrix_4x4::rotation_t matrix_4x4::rotation;
  matrix_4x4::rotation_xyz_t matrix_4x4::rotation_xyz;
  matrix_4x4::rotation_yzx_t matrix_4x4::rotation_yzx;
  matrix_4x4::rotation_yxz_t matrix_4x4::rotation_yxz;

  matrix_4x4::matrix_4x4 (rotation_t, glm::quat const& q)
  {
    _m[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
    _m[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
    _m[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
    _m[0][3] = 0.0f;

    _m[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
    _m[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
    _m[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
    _m[1][3] = 0.0f;

    _m[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
    _m[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
    _m[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
    _m[2][3] = 0.0f;

    _m[3][0] = 0.0f;
    _m[3][1] = 0.0f;
    _m[3][2] = 0.0f;
    _m[3][3] = 1.0f;
  }

  namespace
  {
    enum axis
    {
      x = 0,
      y = 1,
      z = 2,
      num_axis,
    };

    template<axis a>
      matrix_4x4 rotate_axis (radians angle)
    {
      static std::size_t const i_indices[num_axis] = {2, 2, 1};
      static std::size_t const j_indices[num_axis] = {1, 0, 0};
      std::size_t const i_index (i_indices[a]);
      std::size_t const j_index (j_indices[a]);

      float const cosV (glm::cos (angle._));
      float const sinV (glm::sin (angle._));

      matrix_4x4 mat (matrix_4x4::unit);
      mat (j_index, j_index, cosV);
      mat (j_index, i_index, sinV);
      mat (i_index, j_index, -sinV);
      mat (i_index, i_index, cosV);

      return mat;
    }
  }

  matrix_4x4::matrix_4x4 (rotation_xyz_t, degrees::vec3 const& angle)
    : matrix_4x4 (unit)
  {
    *this *= rotate_axis<x>(degrees(angle.x));
    *this *= rotate_axis<y>(degrees(angle.y));
    *this *= rotate_axis<z>(degrees(angle.z));
  }
  matrix_4x4::matrix_4x4 (rotation_yzx_t, degrees::vec3 const& angle)
    : matrix_4x4 (unit)
  {
    *this *= rotate_axis<y> (degrees(angle.y));
    *this *= rotate_axis<z> (degrees(angle.z));
    *this *= rotate_axis<x> (degrees(angle.x));
  }

  matrix_4x4::matrix_4x4(rotation_yxz_t, degrees::vec3 const& angle)
      : matrix_4x4(unit)
  {
    *this *= rotate_axis<y>(degrees(angle.y));
    *this *= rotate_axis<x>(degrees(angle.x));
    *this *= rotate_axis<z>(degrees(angle.z));
  }

  glm::vec3 matrix_4x4::operator* (glm::vec3 const& v) const
  {
    return { _m[0][0] * v[0] + _m[0][1] * v[1] + _m[0][2] * v[2] + _m[0][3]
           , _m[1][0] * v[0] + _m[1][1] * v[1] + _m[1][2] * v[2] + _m[1][3]
           , _m[2][0] * v[0] + _m[2][1] * v[1] + _m[2][2] * v[2] + _m[2][3]
           };
  }
  glm::vec4 matrix_4x4::operator* (const glm::vec4& v) const
  {
    return { _m[0][0] * v[0] + _m[0][1] * v[1] + _m[0][2] * v[2] + _m[0][3] * v[3]
           , _m[1][0] * v[0] + _m[1][1] * v[1] + _m[1][2] * v[2] + _m[1][3] * v[3]
           , _m[2][0] * v[0] + _m[2][1] * v[1] + _m[2][2] * v[2] + _m[2][3] * v[3]
           , _m[3][0] * v[0] + _m[3][1] * v[1] + _m[3][2] * v[2] + _m[3][3] * v[3]
           };
  }

  matrix_4x4 matrix_4x4::operator* (matrix_4x4 const& other) const
  {
    return { _m[0][0] * other._m[0][0] + _m[0][1] * other._m[1][0] + _m[0][2] * other._m[2][0] + _m[0][3] * other._m[3][0]
           , _m[0][0] * other._m[0][1] + _m[0][1] * other._m[1][1] + _m[0][2] * other._m[2][1] + _m[0][3] * other._m[3][1]
           , _m[0][0] * other._m[0][2] + _m[0][1] * other._m[1][2] + _m[0][2] * other._m[2][2] + _m[0][3] * other._m[3][2]
           , _m[0][0] * other._m[0][3] + _m[0][1] * other._m[1][3] + _m[0][2] * other._m[2][3] + _m[0][3] * other._m[3][3]
           , _m[1][0] * other._m[0][0] + _m[1][1] * other._m[1][0] + _m[1][2] * other._m[2][0] + _m[1][3] * other._m[3][0]
           , _m[1][0] * other._m[0][1] + _m[1][1] * other._m[1][1] + _m[1][2] * other._m[2][1] + _m[1][3] * other._m[3][1]
           , _m[1][0] * other._m[0][2] + _m[1][1] * other._m[1][2] + _m[1][2] * other._m[2][2] + _m[1][3] * other._m[3][2]
           , _m[1][0] * other._m[0][3] + _m[1][1] * other._m[1][3] + _m[1][2] * other._m[2][3] + _m[1][3] * other._m[3][3]
           , _m[2][0] * other._m[0][0] + _m[2][1] * other._m[1][0] + _m[2][2] * other._m[2][0] + _m[2][3] * other._m[3][0]
           , _m[2][0] * other._m[0][1] + _m[2][1] * other._m[1][1] + _m[2][2] * other._m[2][1] + _m[2][3] * other._m[3][1]
           , _m[2][0] * other._m[0][2] + _m[2][1] * other._m[1][2] + _m[2][2] * other._m[2][2] + _m[2][3] * other._m[3][2]
           , _m[2][0] * other._m[0][3] + _m[2][1] * other._m[1][3] + _m[2][2] * other._m[2][3] + _m[2][3] * other._m[3][3]
           , _m[3][0] * other._m[0][0] + _m[3][1] * other._m[1][0] + _m[3][2] * other._m[2][0] + _m[3][3] * other._m[3][0]
           , _m[3][0] * other._m[0][1] + _m[3][1] * other._m[1][1] + _m[3][2] * other._m[2][1] + _m[3][3] * other._m[3][1]
           , _m[3][0] * other._m[0][2] + _m[3][1] * other._m[1][2] + _m[3][2] * other._m[2][2] + _m[3][3] * other._m[3][2]
           , _m[3][0] * other._m[0][3] + _m[3][1] * other._m[1][3] + _m[3][2] * other._m[2][3] + _m[3][3] * other._m[3][3]
           };
  }

  std::vector<glm::vec3> matrix_4x4::operator* (std::vector<glm::vec3> points) const
  {
  	auto adjustedPoints = std::vector<glm::vec3>();

    for(auto const &point : points)
    {
        adjustedPoints.push_back(*this * point);
    }

    return adjustedPoints;
  }

  namespace
  {
    float minor_size (matrix_4x4 const& mat, std::size_t x, std::size_t y)
    {
      float s[3][3];
      for (std::size_t j (0), v (0); j < 4; ++j)
      {
        if (j != y)
        {
          for (std::size_t i (0), u (0); i < 4; ++i)
          {
            if (i != x)
            {
              s[v][u++] = mat (j, i);
            }
          }
          ++v;
        }
      }
#define SUB(a,b) (s[1][a] * s[2][b] - s[2][a] * s[1][b])
      return s[0][0] * SUB (1,2) - s[0][1] * SUB (0,2) + s[0][2] * SUB (0,1);
#undef SUB
    }
  }

  matrix_4x4 matrix_4x4::adjoint() const
  {
    return {  minor_size (*this, 0, 0), -minor_size (*this, 0, 1),  minor_size (*this, 0, 2), -minor_size (*this, 0, 3)
           , -minor_size (*this, 1, 0),  minor_size (*this, 1, 1), -minor_size (*this, 1, 2),  minor_size (*this, 1, 3)
           ,  minor_size (*this, 2, 0), -minor_size (*this, 2, 1),  minor_size (*this, 2, 2), -minor_size (*this, 2, 3)
           , -minor_size (*this, 3, 0),  minor_size (*this, 3, 1), -minor_size (*this, 3, 2),  minor_size (*this, 3, 3)
           };
  }

  matrix_4x4& matrix_4x4::operator* (float f)
  {
    for (std::size_t i (0); i < 16; ++i)
    {
      _data[i] *= f;
    }
    return *this;
  }
  matrix_4x4& matrix_4x4::operator/ (float f)
  {
    return *this * (1 / f);
  }

  namespace
  {
    float determinant (matrix_4x4 const& mat)
    {
#define SUB(a, b) (mat (2, a) * mat (3, b) - mat (3, a) * mat (2, b))
      return mat (0, 0) * (mat (1, 1) * SUB (2, 3) - mat (1, 2) * SUB (1, 3) + mat (1, 3) * SUB (1, 2))
           - mat (0, 1) * (mat (1, 0) * SUB (2, 3) - mat (1, 2) * SUB (0, 3) + mat (1, 3) * SUB (0, 2))
           + mat (0, 2) * (mat (1, 0) * SUB (1, 3) - mat (1, 1) * SUB (0, 3) + mat (1, 3) * SUB (0, 1))
           - mat (0, 3) * (mat (1, 0) * SUB (1, 2) - mat (1, 1) * SUB (0, 2) + mat (1, 2) * SUB (0, 1));
#undef SUB
    }
  }

  matrix_4x4 matrix_4x4::inverted() const
  {
    return adjoint() / determinant (*this);
  }

	glm::mat4x4 matrix_4x4::Convert() const
  {
        return { _m[0][0], _m[1][0], _m[2][0], _m[3][0]
           , _m[0][1], _m[1][1], _m[2][1], _m[3][1]
           , _m[0][2], _m[1][2], _m[2][2], _m[3][2]
           , _m[0][3], _m[1][3], _m[2][3], _m[3][3]
        };
  }

    glm::mat4x4 matrix_4x4::ConvertX() const
    {
        return { _m[0][0] , _m[0][1] , _m[0][2] , _m[0][3],
      _m[1][0] , _m[1][1] , _m[1][2] , _m[1][3] ,
      _m[2][0] , _m[2][1] , _m[2][2] , _m[2][3] ,
      _m[3][0] , _m[3][1] , _m[3][2] , _m[3][3] 
        };
    }

  matrix_4x4 matrix_4x4::transposed() const
  {
    return { _m[0][0], _m[1][0], _m[2][0], _m[3][0]
           , _m[0][1], _m[1][1], _m[2][1], _m[3][1]
           , _m[0][2], _m[1][2], _m[2][2], _m[3][2]
           , _m[0][3], _m[1][3], _m[2][3], _m[3][3]
           };
  }
}

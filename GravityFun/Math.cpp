#include "Math.h"

#include <cmath>
#include <utility>

namespace GravityFun::Math
{
    Vec2::Vec2() : x(0), y(0) {}
    Vec2::Vec2(double x, double y) : x(x), y(y) {}

    Vec2 Vec2::operator+(const Vec2& other) const
    {
        return Vec2(x + other.x, y + other.y);
    }
    Vec2 Vec2::operator-(const Vec2& other) const
    {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2& Vec2::operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2& Vec2::operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vec2 Vec2::operator*(const double& other) const
    {
        return Vec2(x * other, y * other);
    }
    Vec2 Vec2::operator/(const double& other) const
    {
        return Vec2(x / other, y / other);
    }
    Vec2& Vec2::operator*=(const double& other)
    {
        x *= other;
        y *= other;
        return *this;
    }
    Vec2& Vec2::operator/=(const double& other)
    {
        x /= other;
        y /= other;
        return *this;
    }

    double Vec2::GetMagnitude() const
    {
        return std::sqrt(x * x + y * y);
    }
    Vec2 Vec2::GetNormalized() const
    {
        return *this / GetMagnitude();
    }
    double Vec2::GetDotProduct(const Vec2& other) const
    {
        return x * other.x + y * other.y;
    }

    Matrix4x4::Matrix4x4()
    {
        for (int column = 0; column < 4; column++)
            for (int row = 0; row < 4; row++)
                Data.push_back(row == column ? 1 : 0);
    }

    Matrix4x4::Matrix4x4(std::vector<float> column_major_vector)
    {
        if (column_major_vector.size() == 16)
        {
            Data = std::move(column_major_vector);
            return;
        }
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                Data.push_back(i == j ? 1 : 0);
    }

    void Matrix4x4::Set(int row, int column, float value)
    {
        Data[(column << 2) + row] = value;
    }

    float Matrix4x4::Get(int row, int column) const
    {
        return Data[(column << 2) + row];
    }

    Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
    {
        std::vector<float> resulting_vector;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                float sum = 0;
                for (int i = 0; i < 4; i++)
                {
                    sum += Get(row, i) * other.Get(i, column);
                }
                resulting_vector.push_back(sum);
            }
        }
        return Matrix4x4(resulting_vector);
    }

    float * Matrix4x4::GetData()
    {
        return Data.data();
    }

    Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
    {
        std::vector<float> data;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                if (row == column)
                {
                    switch (column)
                    {
                    case 0:
                        data.push_back(x);
                        break;
                    case 1:
                        data.push_back(y);
                        break;
                    case 2:
                        data.push_back(z);
                        break;
                    case 3:
                        data.push_back(1);
                        break;
                    }
                }
                else
                    data.push_back(0);
            }
        }
        return Matrix4x4(data);
    }
    Matrix4x4 Matrix4x4::RotationAroundX(float angle)
    {
        std::vector<float> data;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                if (column != 3 && row != 3 && column != 0 && row != 0)
                {
                    data.push_back(
                        row == column ?
                            std::cos(angle)
                            : (
                                row == 1 ?
                                    -std::sin(angle)
                                    : std::sin(angle)
                            )
                    );
                }
                else
                    data.push_back(row == column ? 1 : 0);
            }
        }
        return Matrix4x4(data);
    }
    Matrix4x4 Matrix4x4::RotationAroundY(float angle)
    {
        std::vector<float> data;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                if (column != 3 && row != 3 && column != 1 && row != 1)
                {
                    data.push_back(
                        row == column ?
                            std::cos(angle)
                            : (
                                row == 0 ?
                                    std::sin(angle)
                                    : -std::sin(angle)
                            )
                    );
                }
                else
                    data.push_back(row == column ? 1 : 0);
            }
        }
        return Matrix4x4(data);
    }
    Matrix4x4 Matrix4x4::RotationAroundZ(float angle)
    {
        std::vector<float> data;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                if (column != 3 && row != 3 && column != 2 && row != 2)
                {
                    data.push_back(
                        row == column ?
                            std::cos(angle)
                            : (
                                row == 0 ?
                                    -std::sin(angle)
                                    : std::sin(angle)
                            )
                    );
                }
                else
                    data.push_back(row == column ? 1 : 0);
            }
        }
        return Matrix4x4(data);
    }
    Matrix4x4 Matrix4x4::Translation(float x, float y, float z)
    {
        std::vector<float> data;
        for (int column = 0; column < 4; column++)
        {
            for (int row = 0; row < 4; row++)
            {
                if (row == column)
                {
                    data.push_back(1);
                }
                else if (column == 3)
                {
                    switch (row)
                    {
                    case 0:
                        data.push_back(x);
                        break;
                    case 1:
                        data.push_back(y);
                        break;
                    case 2:
                        data.push_back(z);
                        break;
                    }
                }
                else
                    data.push_back(0);
            }
        }
        return Matrix4x4(data);
    }
    //Matrix4x4 Matrix4x4::Perspective()
    //{
    //    // ...
    //    return Matrix4x4();
    //}
}

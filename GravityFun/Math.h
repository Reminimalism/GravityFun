#pragma once

#include <vector>

namespace GravityFun::Math
{
    class Vec2
    {
    public:
        Vec2();
        Vec2(double x, double y);

        double x;
        double y;

        Vec2 operator+(const Vec2&) const;
        Vec2 operator-(const Vec2&) const;
        Vec2& operator+=(const Vec2&);
        Vec2& operator-=(const Vec2&);
        Vec2 operator*(const double&) const;
        Vec2 operator/(const double&) const;
        Vec2& operator*=(const double&);
        Vec2& operator/=(const double&);

        double GetMagnitude() const;
        Vec2 GetNormalized() const;
        double GetDotProduct(const Vec2&) const;
    };

    class Matrix4x4
    {
    public:
        Matrix4x4();
        /// @param column_major_vector Must have 16 elements, else it will be ignored.
        Matrix4x4(std::vector<float> column_major_vector);
        void Set(int row, int column, float value);
        float Get(int row, int column) const;
        Matrix4x4 operator*(const Matrix4x4&) const;
        float * GetData();

        static Matrix4x4 Scale(float x, float y, float z);
        static Matrix4x4 RotationAroundX(float angle);
        static Matrix4x4 RotationAroundY(float angle);
        static Matrix4x4 RotationAroundZ(float angle);
        static Matrix4x4 Translation(float x, float y, float z);
        //static Matrix4x4 Perspective();
    private:
        std::vector<float> Data;
    };
}

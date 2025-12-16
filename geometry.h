#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

/*vec2, vec3 and vec4 classes with basic vector operations. 
Moreover, we will need small matrices (up to 4x4 maximum) and 
basic operations (mainly, multiplication, transposition and inversion). 
It is handy to be able to invert any square matrix, 
but 3x3 inversion may suffice if you do not wish implement a generic inversion.*/
template<int n> struct vec {
    double data[n] = {0};
    double& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    double result = 0;
    for (int i=0; i<n; i++) result += lhs[i] * rhs[i];
    return result;
}

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> result;
    for (int i=0; i<n; i++) result[i] = lhs[i] + rhs[i];
    return result;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> result;
    for (int i=0; i<n; i++) result[i] = lhs[i] - rhs[i];
    return result;
}

template<int n> vec<n> operator*(const vec<n>& v, const double f) {
    vec<n> result;
    for (int i=0; i<n; i++) result[i] = v[i] * f;
    return result;
}

template<int n> vec<n> operator*(const double f, const vec<n>& v) {
    vec<n> result;
    for (int i=0; i<n; i++) result[i] = v[i] * f;
    return result;
}

template<int n> vec<n> operator/(const vec<n>& v, const double f) {
    vec<n> result;
    for (int i=0; i<n; i++) result[i] = v[i] / f;
    return result;
}


template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
};

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    double  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i)       { assert(i>=0 && i<4); return i ? (1==i ? y : (2==i ? z : w)) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<4); return i ? (1==i ? y : (2==i ? z : w)) : x; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v*v);
}

template<int n> vec<n> normalized(const vec<n>& v) {
    return v / norm(v);
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return {v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

template<int rows, int cols> struct mat {
    vec<cols> data[rows];
    vec<cols>& operator[](const int i)       { assert(i>=0 && i<rows); return data[i]; }
    const vec<cols>& operator[](const int i) const { assert(i>=0 && i<rows); return data[i]; }

    double det() const{
        assert(rows==cols);
        return ::det(*this);
    }

    double cofactor(const int row, const int col) const {
        assert(row==col && row>=2); // rules for cofactor
        mat<rows-1,cols-1> sub;
        for (int i=0, subi=0; i<rows; i++) {
            if (i == row) continue;
            for (int j=0, subj=0; j<cols; j++) {
                if (j == col) continue;
                sub[subi][subj] = data[i][j];
                subj++;
            }
            subi++;
        }
        return ((row+col)%2==0 ? 1 : -1) * sub.det();
    }

    mat<rows,cols> invert_transpose() const {
        mat<rows,cols> a_transpose;
        for (int i=0; i<rows; i++)
            for (int j=0; j<cols; j++) a_transpose[i][j]=cofactor(i,j);
        return a_transpose/(data[0] * a_transpose[0]);// equivalent to det()
    }

    mat<rows,cols> invert() const {
        return invert_transpose().transpose();
    }

    mat<cols,rows> transpose() const {
        mat<cols,rows> result;
        for (int i=0; i<cols; i++){
            for (int j=0; j<rows; j++)
                result[i][j] = data[j][i];
        }
        return result;
    }
};

template<int rows, int cols> vec<rows> operator*(const mat<rows,cols>& m, const vec<cols>& v) {
    vec<rows> result;
    for (int i=0; i<rows; i++)
        result[i] = m[i] * v;
    return result;
}

template<int cols> mat<cols,cols> operator*(const vec<cols>& lhs, const mat<1,cols>& rhs) {
    mat<cols,cols> result;
    for (int i=0; i<cols; i++)
        for (int j=0; j<cols; j++)
            result[i][j] = lhs[i] * rhs[j];
    return result;
}

template<int r1, int c1, int c2> mat<r1,c2> operator*(const mat<r1,c1>& lhs, const mat<c1,c2>& rhs) {
    mat<r1,c2> result;
    for (int i=0; i<r1; i++)
        for (int j=0; j<c2; j++) {
            result[i][j] = 0;
            for (int k=0; k<c1; k++)
                result[i][j] += lhs[i][k] * rhs[k][j];
        }
    return result;
}

template<int rows, int cols> mat<rows,cols> operator*(const mat<rows,cols>& lhs, const double rhs) {
    mat<rows,cols> result;
    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            result[i][j] = lhs[i][j] * rhs;
    return result;
}

template<int rows, int cols> mat<rows,cols> operator/(const double lhs, const mat<rows,cols>& rhs) {
    mat<rows,cols> result;
    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            result[i][j] = rhs[i][j] / lhs;
    return result;
}

template<int rows, int cols> mat<rows,cols> operator+(const mat<rows,cols>& lhs, const mat<rows,cols>& rhs) {
    mat<rows,cols> result;
    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            result[i][j] = lhs[i][j] + rhs[i][j];
    return result;
}

template<int rows, int cols> mat<rows,cols> operator-(const mat<rows,cols>& lhs, const mat<rows,cols>& rhs) {
    mat<rows,cols> result;
    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            result[i][j] = lhs[i][j] - rhs[i][j];
    return result;
}

template<int n> double det(const mat<n,n>& src) { //recursive determinant calculation
    if (n == 1){
        return src[0][0];
    }
    double ret = 0;
    for (int i=0; i<n; i++){
        ret += src[0][i] * src.cofactor(0,i)
    }
    return ret;
}


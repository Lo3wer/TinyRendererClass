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

template <int n> struct determinant;

template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows];
    vec<ncols>& operator[](const int i) { assert(i>=0 && i<nrows); return rows[i]; }
    const vec<ncols>& operator[](const int i) const { assert(i>=0 && i<nrows); return rows[i]; }

    double det() const{
        assert(nrows==ncols);
        return determinant<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        mat<nrows-1,ncols-1> submatrix;
        for(int i=0; i<nrows-1; i++){
            for(int j=0; j<ncols-1; j++){
                submatrix[i][j] = rows[i+int(i>=row)][j+int(j>=col)]; //not included row or column of the cofactor coordinates
            }
        }
        return submatrix.det() * ((row+col)%2 ? -1 : 1);
    }

    mat<nrows,ncols> invert_transpose() const {
        mat<nrows,ncols> a_transpose;
        for (int i=0; i<nrows; i++)
            for (int j=0; j<ncols; j++) a_transpose[i][j]=cofactor(i,j);
        return a_transpose/(rows[0] * a_transpose[0]);// equivalent to det()
    }

    mat<nrows,ncols> invert() const {
        return invert_transpose().transpose();
    }

    mat<ncols,nrows> transpose() const {
        mat<ncols,nrows> result;
        for (int i=0; i<ncols; i++){
            for (int j=0; j<nrows; j++)
                result[i][j] = rows[j][i];
        }
        return result;
    }
};

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows,ncols>& m, const vec<ncols>& v) {
    vec<nrows> result;
    for (int i=0; i<nrows; i++){
        int sum = 0;
        for(int j=0; j<ncols; j++){
            sum+= m[i][j] * v[j];
        }
        result[i] = sum;
    }
    return result;
}

template<int ncols> mat<ncols,ncols> operator*(const vec<ncols>& lhs, const mat<1,ncols>& rhs) {
    mat<ncols,ncols> result;
    for (int i=0; i<ncols; i++)
        for (int j=0; j<ncols; j++)
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

template<int nrows, int ncols> mat<nrows,ncols> operator*(const mat<nrows,ncols>& lhs, const double rhs) {
    mat<nrows,ncols> result;
    for (int i=0; i<nrows; i++)
        for (int j=0; j<ncols; j++)
            result[i][j] = lhs[i][j] * rhs;
    return result;
}

template<int nrows, int ncols> mat<nrows,ncols> operator/(const mat<nrows,ncols>& lhs, const double rhs) {
    mat<nrows,ncols> result;
    for (int i=0; i<nrows; i++)
        for (int j=0; j<ncols; j++)
            result[i][j] = lhs[i][j] / rhs;
    return result;
}

template<int nrows, int ncols> mat<nrows,ncols> operator+(const mat<nrows,ncols>& lhs, const mat<nrows,ncols>& rhs) {
    mat<nrows,ncols> result;
    for (int i=0; i<nrows; i++)
        for (int j=0; j<ncols; j++)
            result[i][j] = lhs[i][j] + rhs[i][j];
    return result;
}

template<int nrows, int ncols> mat<nrows,ncols> operator-(const mat<nrows,ncols>& lhs, const mat<nrows,ncols>& rhs) {
    mat<nrows,ncols> result;
    for (int i=0; i<nrows; i++)
        for (int j=0; j<ncols; j++)
            result[i][j] = lhs[i][j] - rhs[i][j];
    return result;
}

template<int nrows,int ncols> std::ostream& operator<<(std::ostream& out, const mat<nrows,ncols>& m) {
    for (int i=0; i<nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct determinant {
    static double det(const mat<n,n>& src) { //recursive determinant calculation
        // if (n == 1){
        //     return src[0][0]; //need to figure out when i get wifi againnnnn
        // }
        double ret = 0;
        for (int i=0; i<n; i++){
            ret += src[0][i] * src.cofactor(0,i);
        }
        return ret;
    }
};

template<> struct determinant<1> {
    static double det(const mat<1,1>& src) {
        return src[0][0];
    }
};


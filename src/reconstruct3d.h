#ifndef METNUM_TP1_RECONSTRUCT3D_H
#define METNUM_TP1_RECONSTRUCT3D_H


#include "matrix.h"
#include "gauss.h"
#include "calibration.h"

struct options {
    options(bool maskOptimization) : maskOptimization(maskOptimization) {}
    bool maskOptimization;
};

matrix<double> sourceOfLightMatrix(const direction &s1, const direction &s2, const direction &s3) {
    return {
            {s1.x, s1.y, s1.z},
            {s2.x, s2.y, s2.z},
            {s3.x, s3.y, s3.z}
    };
}
/**
 * (5):
 * for each pixel
 *  /s1x s2x s3x \  /mx \    /I1 \
 * | s1y s2y s3y | | my | = | I2 |
 * \ s1z s2z s3z/  \ mz/    \ I3/
 *
 *
 * (6)
 * agarrar el vector de m y a cada componente la dividis por la norma m
 *
 * Construction of the normal field, for each pixel
 *
 * @param i1, i2, i3 the matrix with the brightness on each pixel for the 3 images
 * @param s1, s2, s3 the source of light for the images
 * @return the normal field of the image
 */
matrix<row<double>> normalField(const matrix<double> &i1, const matrix<double> &i2, const matrix<double> &i3,
                                const direction &s1, const direction &s2, const direction &s3, options opts) {
    size_t height = i1.size(), width = i1[0].size();
    PLUMatrix<double> sm = gaussian_elimination(sourceOfLightMatrix(s1, s2, s3));
    matrix<row<double>> normal;

    for (size_t i = 0; i < height; i++) {
        row<row<double>> r;
        for (size_t j = 0; j < width; j++) {
            if (opts.maskOptimization && (i1[i][j] == 0 && i2[i][j] == 0 && i3[i][j] == 0)) {
                r.push_back({0, 0, 0}); //TODO: revisen esto
            } else {
                //(5)
                row<double> m = Matrix::solvePLUSystem(sm.P, sm.L, sm.U, {i1[i][j], i2[i][j], i3[i][j]});
                //(6)
                double mNorm = Matrix::twoNorm(m);
                m[0] /= mNorm;
                m[1] /= mNorm;
                m[2] /= mNorm;
                r.push_back(m);
            }
        }
        normal.push_back(r);
    }
    return normal;
}

matrix<double> calculateM(const matrix<row<double>> &n) {
    size_t height = n.size(), width = n[0].size(), n_size = width * height;
    matrix<double> M;
    size_t n_i = 0;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            row<double> r(n_size, 0);
            r[n_i] = -n[y][x][2]; //le pongo -nz
            r[n_i+1] = n[y][x][2]; //le pongo nz
            n_i++;
            M.push_back(r);
        }
    }
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            row<double> r(n_size, 0);
            r[n_i] = -n[y][x][2]; //le pongo -nz
            r[n_i+height] = n[y][x][2]; //le pongo nz
            n_i++;
            M.push_back(r);
        }
    }
    return M;
}

vector<double> calculateV(const matrix<row<double>> &n) {
    size_t height = n.size(), width = n[0].size();
    vector<double> v;
    size_t n_i = 0;
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            v.push_back(-n[y][x][1]); //le pongo -ny
        }
    }
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            v.push_back(-n[y][x][0]); //le pongo -nx
        }
    }
    return v;
}

//Aqui viene lo bueno jovenes, I cho cho choleskyou
matrix<double> findDepth(const matrix<row<double>> &m) {
    assert(false);
    size_t height = m.size(), width = m[0].size();
    matrix<double> depth;
    // TODO: Aca hay que hacer lo que no se que hacer
    return depth;
}

#endif //METNUM_TP1_RECONSTRUCT3D_H

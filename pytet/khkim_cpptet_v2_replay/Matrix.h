#pragma once
#include <iostream>
#include <cstdlib>

#include <vector>

using namespace std;

typedef vector<vector<int>> Array2D;

class Matrix {
private:
  int dy;
  int dx;
  int **array;
  void alloc(int cy, int cx);
public:
  int get_dy();
  int get_dx();
  int** get_array();
  Matrix();
  Matrix(int cy, int cx);
  Matrix(const Matrix *obj);
  Matrix(const Matrix &obj);
  Matrix(int *arr, int col, int row);
  Matrix(Array2D& arr, int col, int row);
  Matrix(int cy, int cx, int n);
  ~Matrix();
  Matrix *clip(int top, int left, int bottom, int right);
  void paste(const Matrix *obj, int top, int left);
  Matrix *add(const Matrix *obj);
  int sum();
  void mulc(int coef);
  Matrix *binary();
  bool anyGreaterThan(int val);
  void print();
  friend ostream& operator<<(ostream& out, const Matrix& obj);
  Matrix& operator=(const Matrix& obj);
};

#include "mathlib.h"
#include "utils.h"

namespace
{
const double inv3 = 1.0 / 3.0;
const double root3 = std::sqrt(3.0);

float eigenVals[3] = {0.0f};

inline bbk::Vector3 GetComplement1(const bbk::Vector3& u, const bbk::Vector3& v) {
	return u.Cross(v).Normalise();
}
void GetComplement2(const bbk::Vector3& u, bbk::Vector3& v, bbk::Vector3& w);
int ComputeRank(bbk::Matrix3x3& mtx);

inline void ROTATE(double mtx[3][3], const double s, const double tau, const int i, const int j, const int k, const int l) {
	double g = mtx[i][j];
	double h = mtx[k][l];
	mtx[i][j] = g - s*(h + g*tau);
	mtx[k][l] = h + s*(g - h*tau);
}
} // anon namespace

namespace bbk
{
void ComputeRoots(const Matrix3x3& mtx, double roots[3])
{
	Matrix3x3 m(mtx);
	float maxElem = std::fabs(mtx.elements[0]);
	for (size_t i = 1; i < 9; ++i)
	{
		float val = std::fabs(mtx.elements[i]);
		if (val > maxElem)
			maxElem = val;
	}
	if (maxElem > 1.0f)
	{
		m = m * (1.0f / maxElem);
	}

	double m00 = m.elements[0];
	double m01 = m.elements[3];
	double m02 = m.elements[6];
	double m11 = m.elements[4];
	double m12 = m.elements[7];
	double m22 = m.elements[8];
	double c0 = m00*m11*m22 + 2.0*m01*m02*m12 - m00*m12*m12 - m11*m02*m02 - m22*m01*m01;
	double c1 = m00*m11 - m01*m01 + m00*m22 - m02*m02 + m11*m22 - m12*m12;
	double c2 = m00 + m11 + m22;
	double c2div3 = c2*inv3;
	double adiv3 = (c1 - c2*c2div3)*inv3;

	if (adiv3 > 0.0f)
		adiv3 = 0.0;

	double mbdiv2 = 0.5*(c0 + c2div3*(2.0*c2div3*c2div3 - c1));
	double q = mbdiv2*mbdiv2 + adiv3*adiv3*adiv3;

	if (q > 0.0)
		q = 0.0;

	double mag = std::sqrt(-adiv3);
	double angle = std::atan2(std::sqrt(-q), mbdiv2) * inv3;
	double cosine = std::cos(angle);
	double sine = std::sin(angle);

	roots[0] = c2div3 + 2.0*mag*cosine;
	roots[1] = c2div3 - mag*(cosine + root3*sine);
	roots[2] = c2div3 - mag*(cosine - root3*sine);

	if (roots[0] > roots[1])
		bbk::utils::swap(roots[0], roots[1]);
	if (roots[1] > roots[2])
		bbk::utils::swap(roots[1], roots[2]);
	if (roots[0] > roots[1])
		bbk::utils::swap(roots[0], roots[1]);
}

void EigenSolver(const Matrix3x3& mtx, float* eigenValues, Vector3* eigenVecs)
{
	double roots[3] = {0.0};
	ComputeRoots(mtx, roots);
	eigenValues[0] = static_cast<float>(roots[0]);
	eigenValues[1] = static_cast<float>(roots[1]);
	eigenValues[2] = static_cast<float>(roots[2]);

	Matrix3x3 m0 = mtx - (Matrix3x3::IDENTITY * eigenValues[0]);
	int m0rank = ::ComputeRank(m0);
	if (m0rank == 0)
	{
		eigenVecs[0] = Vector3(1.0f, 0.0f, 0.0f);
		eigenVecs[1] = Vector3(0.0f, 1.0f, 0.0f);
		eigenVecs[2] = Vector3(0.0f, 0.0f, 1.0f);
		return;
	}
	else if (m0rank == 1)
	{
		GetComplement2(m0.GetRow(0), eigenVecs[0], eigenVecs[1]);
		eigenVecs[2] = eigenVecs[0].Cross(eigenVecs[1]);
		return;
	}

	// m0rank == 2
	eigenVecs[0] = GetComplement1(m0.GetRow(0), m0.GetRow(1));

	Matrix3x3 diag(
		eigenValues[1], 0.0f, 0.0f,
		0.0f, eigenValues[1], 0.0f,
		0.0f, 0.0f, eigenValues[1]);

	Matrix3x3 m1 = mtx - diag;//(Matrix3x3::IDENTITY * eigenValues[1]);
	int m1rank = ComputeRank(m1);
	if (m1rank == 1)
	{
		GetComplement2(eigenVecs[0], eigenVecs[1], eigenVecs[2]);
		return;
	}

	// m1rank == 2
	GetComplement2(eigenVecs[0], eigenVecs[1], eigenVecs[2]);
	//eigenVecs[1] = GetComplement1(m1.GetRow(0), m1.GetRow(1));

	//eigenVecs[2] = eigenVecs[0].Cross(eigenVecs[1]);
}

void GetEigenVecs(const Matrix3x3& mtx, Vector3* eigenVecs)
{
	EigenSolver(mtx, ::eigenVals, eigenVecs);
}

int JacobiSolver3(const Matrix3x3& covar, double eigenvals[3], double eigenvecs[3][3])
{
	double mtx[3][3] = {{0.0f}};
	double thresh, theta, tau, t, s, c, h, g;
	double b[3] = {0.0f};
	double z[3] = {0.0f};
	int numRot = 0;
	int row = 0, col = 0;

	// Set eigenvecs to identity
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
			eigenvecs[i][j] = 0.0;
		eigenvecs[i][i] = 1.0;
	}

	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 3; ++j)
			mtx[i][j] = covar.m[i][j];

	// Initialise b and eigenvals to the diagonal of covar
	b[0] = mtx[0][0];
	b[1] = mtx[1][1];
	b[2] = mtx[2][2];
	eigenvals[0] = static_cast<float>(mtx[0][0]);
	eigenvals[1] = static_cast<float>(mtx[1][1]);
	eigenvals[2] = static_cast<float>(mtx[2][2]);
	
	for (size_t i = 0; i < 50; ++i)
	{
		// Sum off-diagonal elements
		double offDiagSum = std::abs(mtx[0][1]) + std::abs(mtx[0][2]) + std::abs(mtx[1][2]);
		if (offDiagSum < bbk::EPSILONf)
			return numRot;

		thresh = i < 4 ? thresh = 0.2f * offDiagSum / 9 : 0.0;

		for (col = 0; col < 2; ++col)
		{
			for (row = col + 1; row < 3; ++row)
			{
				g = 100.0 * std::abs(mtx[col][row]);
				if (i > 4 && g <= bbk::EPSILONf*std::abs(eigenvals[col]) && g <= bbk::EPSILONf*std::abs(eigenvals[row]))
				{
					mtx[col][row] = 0.0;
				}
				else if (std::abs(mtx[col][row]) > thresh)
				{
					h = eigenvals[row] - eigenvals[col];
					if (g <= bbk::EPSILONf*std::abs(h))
					{
						t = mtx[col][row] / h;
					}
					else
					{
						theta = 0.5 * h / mtx[col][row];
						t = 1.0f / (std::abs(theta) + std::sqrt(1.0 + theta*theta));
						if (theta < 0.0)
							t = -t;
					}
					c = 1.0 / std::sqrt(1 + t*t);
					s = t*c;
					tau = s / (1.0 + c);
					h = t*mtx[col][row];
					z[col] -= h;
					z[row] += h;
					eigenvals[col] -=h;
					eigenvals[row] += h;
					mtx[col][row] = 0.0f;
					for (int j = 0; j < col; ++j)
						::ROTATE(mtx, s, tau, j, col, j, row);
					for (int j = col + 1; j < row; ++j)
						::ROTATE(mtx, s, tau, col, j, j, row);
					for (int j = row + 1; j < 3; ++j)
						::ROTATE(mtx, s, tau, col, j, row, j);
					for (int j = 0; j < 3; ++j)
						::ROTATE(eigenvecs, s, tau, j, col, j, row);
					++numRot;
				}
			}
		}
		for (col = 0; col < 3; ++col)
		{
			b[col] += z[col];
			eigenvals[col] = b[col];
			z[col] = 0.0;
		}
	}
	return -1;
}
} // namespace bbk

namespace
{
void GetComplement2(const bbk::Vector3& u, bbk::Vector3& v, bbk::Vector3& w)
{
	const bbk::Vector3 nrm(u.Normalise());
	if (std::fabs(nrm.x) >= std::fabs(nrm.y))
	{
		float invLen = 1.0f / std::sqrt(nrm.x*nrm.x + nrm.z*nrm.z);
		v.x = -nrm.z * invLen;
		v.y = 0.0f;
		v.z = nrm.x * invLen;
		w.x = nrm.y * v.z;
		w.y = nrm.z * v.x - nrm.x * v.z;
		w.z = -nrm.y * v.x;
	}
	else
	{
		float invLen = 1.0f / std::sqrt(nrm.y*nrm.y + nrm.z*nrm.z);
		v.x = 0.0f;
		v.y = nrm.z * invLen;
		v.z = -nrm.y * invLen;
		w.x = nrm.y * v.z - nrm.z * v.y;
		w.y = -nrm.x * v.z;
		w.z = nrm.x * v.y;
	}
}

int ComputeRank(bbk::Matrix3x3& mtx)
{
	float abs, max = -1.0f;
	int row, col, maxRow = -1, maxCol = -1;

	for (row = 0; row < 3; ++row)
	{
		for (col = row; col < 3; ++col)
		{
			abs = std::fabs(mtx.m[col][row]);
			if (abs > max)
			{
				max = abs;
				maxRow = row;
				maxCol = col;
			}
		}
	}

	if (std::fabs(max) < bbk::EPSILONf) // Rank is 0
	{
		return 0;
	}

	if (maxRow != 0) // Rank is at least 1
	{
		for (col = 0; col < 3; ++col)
		{
			bbk::utils::swap(mtx.m[col][0], mtx.m[col][maxRow]);
		}
	}

	// Row reduction
	float invMax = 1.0f / mtx.m[maxCol][0];
	mtx.m[0][0] *= invMax;
	mtx.m[1][0] *= invMax;
	mtx.m[2][0] *= invMax;

	// Eliminate maxCol column entries in rows 1 and 2
	if (maxCol == 0)
	{
		mtx.m[1][1] -= mtx.m[0][1] * mtx.m[1][0];
		mtx.m[2][1] -= mtx.m[0][1] * mtx.m[2][0];
		mtx.m[1][2] -= mtx.m[0][2] * mtx.m[1][0];
		mtx.m[2][2] -= mtx.m[0][2] * mtx.m[2][0];
		mtx.m[0][1] = 0.0f;
		mtx.m[0][2] = 0.0f;
	}
	else if (maxCol == 1)
	{
		mtx.m[0][1] -= mtx.m[1][1] * mtx.m[0][0];
		mtx.m[2][1] -= mtx.m[1][1] * mtx.m[2][0];
		mtx.m[0][2] -= mtx.m[1][2] * mtx.m[0][0];
		mtx.m[2][2] -= mtx.m[1][2] * mtx.m[2][0];
		mtx.m[1][1] = 0.0f;
		mtx.m[1][2] = 0.0f;
	}
	else // maxCol == 2
	{
		mtx.m[0][1] -= mtx.m[2][1] * mtx.m[0][0];
		mtx.m[1][1] -= mtx.m[2][1] * mtx.m[1][0];
		mtx.m[0][2] -= mtx.m[2][2] * mtx.m[0][0];
		mtx.m[1][2] -= mtx.m[2][2] * mtx.m[1][0];
		mtx.m[2][1] = 0.0f;
		mtx.m[2][2] = 0.0f;
	}

	// Compute max mag entry in last 2 rows
	max = -1.0f;
	maxRow = -1;
	maxCol = -1;
	for (row = 1; row < 3; ++row)
	{
		for (col = 0; col < 3; ++col)
		{
			abs = std::fabs(mtx.m[col][row]);
			if (abs > max)
			{
				max = abs;
				maxRow = row;
				maxCol = col;
			}
		}
	}
	if (std::fabs(max) < bbk::EPSILONf)
	{
		return 1;
	}

	if (maxRow == 2)
	{
		for (col = 0; col < 3; ++col)
		{
			bbk::utils::swap(mtx.m[col][1], mtx.m[col][2]);
		}
	}

	// Scale row 1 to get 1 valued pivot
	invMax = 1.0f / mtx.m[maxCol][1];
	mtx.m[0][1] *= invMax;
	mtx.m[1][1] *= invMax;
	mtx.m[2][1] *= invMax;

	return 2;
}
} // anon namespace

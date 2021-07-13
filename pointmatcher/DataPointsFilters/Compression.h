#pragma once

#include "PointMatcher.h"

template<typename T>
struct CompressionDataPointsFilter : public PointMatcher<T>::DataPointsFilter
{
	typedef PointMatcher<T> PM;
	typedef typename PM::Matrix Matrix;
	typedef typename PM::Vector Vector;

	typedef PointMatcherSupport::Parametrizable Parametrizable;
	typedef Parametrizable P;
	typedef Parametrizable::Parameters Parameters;
	typedef Parametrizable::ParametersDoc ParametersDoc;

	inline static const std::string description()
	{
		return "Lossy point cloud compression using descriptive statistics."
			   "Required descriptors: none.\n"
			   "Produced descritors:  covariance, weightSum, nbPoints, normals, eigValues, eigVectors.\n"
			   "Altered descriptors:  all.\n"
			   "Altered features:     points coordinates and number of points.";
	}

	inline static const ParametersDoc availableParameters()
	{
		return {
				{"knn",               "number of nearest neighbors to consider in the reference",                        "10",            "1",    "2147483647",
						&P::Comp < unsigned > },
				{"maxDist",           "maximum distance to consider for neighbors",                                      "inf",           "0",    "inf",
						&P::Comp < T > },
				{"epsilon",           "Step of discretization for the angle spaces",                                     "0.09817477042", "0",    "3.14159265359",
						&P::Comp < T > },
				{"maxIterationCount", "Maximum number of iterations",                                                    "5",             "0",    "2147483647",
						&P::Comp < unsigned > },
				{"initialVariance",   "Variance on individual point positions (isotropic)",                              "9e-4",          "1e-6", "inf",
						&P::Comp < T > },
				{"maxDeviation",      "Maximum distance from the mean for a point to represent a distribution.",         "0.3",           "0.0",  "inf",
						&P::Comp < T > },
				{"keepNormals",       "whether the normals should be added as descriptors to the resulting cloud",       "0"},
				{"keepEigenValues",   "whether the eigen values should be added as descriptors to the resulting cloud",  "0"},
				{"keepEigenVectors",  "whether the eigen vectors should be added as descriptors to the resulting cloud", "0"}
		};
	}

	const unsigned knn;
	const T maxDist;
	const T epsilon;
	const unsigned maxIterationCount;
	const T initialVariance;
	const T maxDeviation;
	const bool keepNormals;
	const bool keepEigenValues;
	const bool keepEigenVectors;

	CompressionDataPointsFilter(const Parameters& params = Parameters());
	virtual typename PM::DataPoints filter(const typename PM::DataPoints& input);
	virtual void inPlaceFilter(typename PM::DataPoints& cloud);
};

// kate: replace-tabs off; indent-width 4; indent-mode normal
// vim: ts=4:sw=4:noexpandtab
/*

Copyright (c) 2010--2011,
François Pomerleau and Stephane Magnenat, ASL, ETHZ, Switzerland
You can contact the authors at <f dot pomerleau at gmail dot com> and
<stephane at magnenat dot net>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ETH-ASL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __POINTMATCHER_DATAPOINTSFILTERS_H
#define __POINTMATCHER_DATAPOINTSFILTERS_H

#include "Core.h"

template<typename T>
struct DataPointsFiltersImpl
{
	typedef PointMatcherSupport::Parametrizable Parametrizable;
	typedef PointMatcherSupport::Parametrizable P;
	typedef Parametrizable::Parameters Parameters;
	typedef Parametrizable::ParameterDoc ParameterDoc;
	typedef Parametrizable::ParametersDoc ParametersDoc;
	typedef Parametrizable::InvalidParameter InvalidParameter;
	
	typedef typename PointMatcher<T>::Vector Vector;
	typedef typename PointMatcher<T>::Matrix Matrix;	
	typedef typename PointMatcher<T>::DataPoints DataPoints;
	typedef typename PointMatcher<T>::DataPointsFilter DataPointsFilter;
	
	//! Identidy, does nothing
	struct IdentityDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "does nothing";
		}
		
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Subsampling. Filter points beyond a maximum distance measured on a specific axis
	struct MaxDistDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. Filter points beyond a maximum distance measured on a specific axis.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "dim", "dimension on which the filter will be applied. x=0, y=1, z=2, radius=-1", "-1", "-1", "2", &P::Comp<int> },
				{ "maxDist", "maximum distance authorized. All points beyond that will be filtered.", "1", "0", "inf", P::Comp<T> }
			});
		}

		const int dim;
		const T maxDist;
		
		//! Constructor, uses parameter interface
		MaxDistDataPointsFilter(const Parameters& params = Parameters());
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Subsampling. Filter points before a minimum distance measured on a specific axis
	struct MinDistDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. Filter points before a minimum distance measured on a specific axis.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "dim", "dimension on which the filter will be applied. x=0, y=1, z=2, all=3", "3", "0", "2147483647", &P::Comp<unsigned> },
				{ "minDist", "minimum distance authorized. All points before that will be filtered.", "1", "0", "inf", &P::Comp<T> }
			});
		}
		
		const unsigned dim;
		const T minDist;
		
		//! Constructor, uses parameter interface
		MinDistDataPointsFilter(const Parameters& params = Parameters());
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Subsampling. Filter points beyond a maximum quantile measured on a specific axis
	struct MaxQuantileOnAxisDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. Filter points beyond a maximum quantile measured on a specific axis.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "dim", "dimension on which the filter will be applied. x=0, y=1, z=2", "0", "0", "2147483647", &P::Comp<unsigned> },
				{ "ratio", "maximum quantile authorized. All points beyond that will be filtered.", "0.5", "0.0000001", "0.9999999", &P::Comp<T>}
			});
		}
		
		const unsigned dim;
		const T ratio;
		
		//! Constructor, uses parameter interface
		MaxQuantileOnAxisDataPointsFilter(const Parameters& params = Parameters());
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Subsampling. Reduce the points number of a certain ration while trying to uniformize the density of the point cloud.
	struct UniformizeDensityDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. Reduce the points number of a certain ration while trying to uniformize the density of the point cloud.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{"ratio", "targeted reduction ratio", "0.5", "0.0000001", "0.9999999", &P::Comp<T>},
				{ "nbBin", "number of bin used to estimate the probability distribution of the density.", "1", "1", "2147483647", &P::Comp<unsigned> }
			});
		}
		
		const T ratio;
		const unsigned nbBin;
		
		//! Constructor, uses parameter interface
		UniformizeDensityDataPointsFilter(const Parameters& params = Parameters());
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Surface normals estimation. Find the normal for every point using eigen-decomposition of neighbour points
	struct SurfaceNormalDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Normals. This filter extracts the normal to each point by taking the eigenvector corresponding to the smallest eigenvalue of its nearest neighbors.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "knn", "number of nearest neighbors to consider, including the point itself", "5", "3", "2147483647", &P::Comp<unsigned> },
				{ "epsilon", "approximation to use for the nearest-neighbor search", "0", "0", "inf", &P::Comp<T> },
				{ "keepNormals", "whether the normals should be added as descriptors to the resulting cloud", "1"},
				{ "keepDensities", "whether the point densities should be added as descriptors to the resulting cloud", "0"},
				{ "keepEigenValues", "whether the eigen values should be added as descriptors to the resulting cloud", "0" },
				{ "keepEigenVectors", "whether the eigen vectors should be added as descriptors to the resulting cloud", "0" },
				{ "keepMatchedIds" , "whethen the identifiers of matches points should be added as descriptors to the resulting cloud", "0" }
			});
		}
		
		const unsigned knn;
		const double epsilon;
		const bool keepNormals;
		const bool keepDensities;
		const bool keepEigenValues;
		const bool keepEigenVectors;
		const bool keepMatchedIds;

		SurfaceNormalDataPointsFilter(const Parameters& params = Parameters());
		virtual ~SurfaceNormalDataPointsFilter() {};
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Sampling surface normals. First decimate the space until there is at most binSize points, then find the center of mass and use the points to estimate nromal using eigen-decomposition
	struct SamplingSurfaceNormalDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling, Normals. This filter decomposes the point-cloud space in boxes, by recursively splitting the cloud through axis-aligned hyperplanes such as to maximize the evenness of the aspect ratio of the box. When the number of points in a box reaches a value binSize or lower, the filter computes the center of mass of these points and its normal by taking the eigenvector corresponding to the smallest eigenvalue of all points in the box.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "binSize", "limit over which a box is splitted in two", "7", "3", "2147483647", &P::Comp<unsigned> },
				{ "averageExistingDescriptors", "whether the filter keep the existing point descriptors and average them or should it drop them", "1" },
				{ "keepNormals", "whether the normals should be added as descriptors to the resulting cloud", "1" },
				{ "keepDensities", "whether the point densities should be added as descriptors to the resulting cloud", "0" },
				{ "keepEigenValues", "whether the eigen values should be added as descriptors to the resulting cloud", "0" },
				{ "keepEigenVectors", "whether the eigen vectors should be added as descriptors to the resulting cloud", "0" }
			});
		}
		
		const unsigned binSize;
		const bool averageExistingDescriptors;
		const bool keepNormals;
		const bool keepDensities;
		const bool keepEigenValues;
		const bool keepEigenVectors;
		
	public:
		SamplingSurfaceNormalDataPointsFilter(const Parameters& params = Parameters());
		virtual ~SamplingSurfaceNormalDataPointsFilter() {}
		virtual DataPoints filter(const DataPoints& input);
		
	protected:
		struct BuildData
		{
			typedef std::vector<int> Indices;
			
			Indices indices;
			const Matrix& inputFeatures;
			const Matrix& inputDescriptors;
			Matrix outputFeatures;
			Matrix outputDescriptors;
			int outputInsertionPoint;
			
			BuildData(const Matrix& inputFeatures, const Matrix& inputDescriptors, const int finalDescDim):
				inputFeatures(inputFeatures),
				inputDescriptors(inputDescriptors),
				outputFeatures(inputFeatures.rows(), inputFeatures.cols()),
				outputDescriptors(finalDescDim, inputFeatures.cols()),
				outputInsertionPoint(0)
			{
				const int pointsCount(inputFeatures.cols());
				indices.reserve(pointsCount);
				for (int i = 0; i < pointsCount; ++i)
					indices[i] = i;
			}
		};
		
		struct CompareDim
		{
			const int dim;
			const BuildData& buildData;
			CompareDim(const int dim, const BuildData& buildData):dim(dim),buildData(buildData){}
			bool operator() (const int& p0, const int& p1)
			{
				return  buildData.inputFeatures(dim, p0) < 
						buildData.inputFeatures(dim, p1);
			}
		};
		
	protected:
		void buildNew(BuildData& data, const int first, const int last, const Vector minValues, const Vector maxValues) const;
		void fuseRange(BuildData& data, const int first, const int last) const;
	};

	//! Reorientation of normals
	struct OrientNormalsDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Normals. Reorient normals so that they all point in the same direction, with respect to coordinate 0.";
		}
		
		virtual DataPoints filter(const DataPoints& input);
	};

	//! Random sampling
	struct RandomSamplingDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. This filter reduces the size of the point cloud by randomly dropping points.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "prob", "probability to keep a point, one over decimation factor ", "0.75", "0", "1", &P::Comp<T> }
			});
		}
		
		const double prob;
		
		RandomSamplingDataPointsFilter(const Parameters& params = Parameters());
		virtual ~RandomSamplingDataPointsFilter() {};
		virtual DataPoints filter(const DataPoints& input);
		
	private:
		DataPoints randomSample(const DataPoints& input) const;
	};

	//! Systematic sampling, with variation over time
	struct FixstepSamplingDataPointsFilter: public DataPointsFilter
	{
		inline static const std::string description()
		{
			return "Subsampling. This filter reduces the size of the point cloud by only keeping one point over step ones; with step varying in time from startStep to endStep, each iteration getting multiplied by stepMult.";
		}
		inline static const ParametersDoc availableParameters()
		{
			return ParametersDoc({
				{ "startStep", "initial number of point to skip (initial decimation factor)", "10", "0.0000001", "inf", &P::Comp<T> },
				{ "endStep", "maximal or minimal number of points to skip (final decimation factor)", "10", "0.0000001", "inf", &P::Comp<T> },
				{ "stepMult", "multiplication factor to compute the new decimation factor for each iteration", "1", "0.0000001", "inf", &P::Comp<T> }
			});
		}
		
		// number of steps to skip
		const double startStep;
		const double endStep;
		const double stepMult;

	protected:
		double step;
		
	public:
		FixstepSamplingDataPointsFilter(const Parameters& params = Parameters());
		virtual ~FixstepSamplingDataPointsFilter() {};
		virtual void init();
		virtual DataPoints filter(const DataPoints& input);
	private:
		DataPoints fixstepSample(const DataPoints& input);
	};
}; // DataPointsFiltersImpl

#endif // __POINTMATCHER_DATAPOINTSFILTERS_H
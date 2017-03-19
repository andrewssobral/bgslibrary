/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <exception>

#include <opencv2/opencv.hpp>
#include "np_opencv_converter.h"

#include "../package_bgs/bgslibrary.h"

namespace py = boost::python;

cv::Mat test_transpose(const cv::Mat& in)
{
	std::cerr << "Input size: " << in.size() << std::endl;
	std::cerr << "Returning transpose" << std::endl;
	return in.t();
}

namespace fs 
{ 
	namespace python 
	{
		BOOST_PYTHON_MODULE(libbgs)
		{
			// Main types export
			fs::python::init_and_export_converters();
			py::scope scope = py::scope();
			
			// Basic test
			py::def("test_transpose", &test_transpose);
			
			py::class_<FrameDifference>("FrameDifference")
			.def("apply", &FrameDifference::apply)
			.def("getBackgroundModel", &FrameDifference::getBackgroundModel)
			;

			py::class_<StaticFrameDifference>("StaticFrameDifference")
				.def("apply", &StaticFrameDifference::apply)
				.def("getBackgroundModel", &StaticFrameDifference::getBackgroundModel)
				;

			py::class_<AdaptiveBackgroundLearning>("AdaptiveBackgroundLearning")
				.def("apply", &AdaptiveBackgroundLearning::apply)
				.def("getBackgroundModel", &AdaptiveBackgroundLearning::getBackgroundModel)
				;

			py::class_<AdaptiveSelectiveBackgroundLearning>("AdaptiveSelectiveBackgroundLearning")
				.def("apply", &AdaptiveSelectiveBackgroundLearning::apply)
				.def("getBackgroundModel", &AdaptiveSelectiveBackgroundLearning::getBackgroundModel)
				;

			py::class_<DPAdaptiveMedian>("DPAdaptiveMedian")
				.def("apply", &DPAdaptiveMedian::apply)
				.def("getBackgroundModel", &DPAdaptiveMedian::getBackgroundModel)
				;

			py::class_<DPEigenbackground>("DPEigenbackground")
				.def("apply", &DPEigenbackground::apply)
				.def("getBackgroundModel", &DPEigenbackground::getBackgroundModel)
				;

			py::class_<DPGrimsonGMM>("DPGrimsonGMM")
				.def("apply", &DPGrimsonGMM::apply)
				.def("getBackgroundModel", &DPGrimsonGMM::getBackgroundModel)
				;

			py::class_<DPMean>("DPMean")
				.def("apply", &DPMean::apply)
				.def("getBackgroundModel", &DPMean::getBackgroundModel)
				;

			py::class_<DPPratiMediod>("DPPratiMediod")
				.def("apply", &DPPratiMediod::apply)
				.def("getBackgroundModel", &DPPratiMediod::getBackgroundModel)
				;

			py::class_<DPTexture>("DPTexture")
				.def("apply", &DPTexture::apply)
				.def("getBackgroundModel", &DPTexture::getBackgroundModel)
				;

			py::class_<DPWrenGA>("DPWrenGA")
				.def("apply", &DPWrenGA::apply)
				.def("getBackgroundModel", &DPWrenGA::getBackgroundModel)
				;

			py::class_<DPZivkovicAGMM>("DPZivkovicAGMM")
				.def("apply", &DPZivkovicAGMM::apply)
				.def("getBackgroundModel", &DPZivkovicAGMM::getBackgroundModel)
				;

			py::class_<FuzzyChoquetIntegral>("FuzzyChoquetIntegral")
				.def("apply", &FuzzyChoquetIntegral::apply)
				.def("getBackgroundModel", &FuzzyChoquetIntegral::getBackgroundModel)
				;

			py::class_<FuzzySugenoIntegral>("FuzzySugenoIntegral")
				.def("apply", &FuzzySugenoIntegral::apply)
				.def("getBackgroundModel", &FuzzySugenoIntegral::getBackgroundModel)
				;

#if CV_MAJOR_VERSION == 2
			py::class_<GMG>("GMG")
				.def("apply", &GMG::apply)
				.def("getBackgroundModel", &GMG::getBackgroundModel)
				;
#endif

			py::class_<IndependentMultimodal>("IndependentMultimodal")
				.def("apply", &IndependentMultimodal::apply)
				.def("getBackgroundModel", &IndependentMultimodal::getBackgroundModel)
				;

			py::class_<KDE>("KDE")
				.def("apply", &KDE::apply)
				.def("getBackgroundModel", &KDE::getBackgroundModel)
				;

#if CV_MAJOR_VERSION == 3
			py::class_<KNN>("KNN")
				.def("apply", &KNN::apply)
				.def("getBackgroundModel", &KNN::getBackgroundModel)
				;
#endif

			py::class_<LBAdaptiveSOM>("LBAdaptiveSOM")
				.def("apply", &LBAdaptiveSOM::apply)
				.def("getBackgroundModel", &LBAdaptiveSOM::getBackgroundModel)
				;

			py::class_<LBFuzzyAdaptiveSOM>("LBFuzzyAdaptiveSOM")
				.def("apply", &LBFuzzyAdaptiveSOM::apply)
				.def("getBackgroundModel", &LBFuzzyAdaptiveSOM::getBackgroundModel)
				;

			py::class_<LBFuzzyGaussian>("LBFuzzyGaussian")
				.def("apply", &LBFuzzyGaussian::apply)
				.def("getBackgroundModel", &LBFuzzyGaussian::getBackgroundModel)
				;

			py::class_<LBMixtureOfGaussians>("LBMixtureOfGaussians")
				.def("apply", &LBMixtureOfGaussians::apply)
				.def("getBackgroundModel", &LBMixtureOfGaussians::getBackgroundModel)
				;

			py::class_<LBSimpleGaussian>("LBSimpleGaussian")
				.def("apply", &LBSimpleGaussian::apply)
				.def("getBackgroundModel", &LBSimpleGaussian::getBackgroundModel)
				;

			py::class_<LBP_MRF>("LBP_MRF")
				.def("apply", &LBP_MRF::apply)
				.def("getBackgroundModel", &LBP_MRF::getBackgroundModel)
				;

			py::class_<LOBSTER>("LOBSTER")
				.def("apply", &LOBSTER::apply)
				.def("getBackgroundModel", &LOBSTER::getBackgroundModel)
				;

#if CV_MAJOR_VERSION == 2
			py::class_<MixtureOfGaussianV1>("MixtureOfGaussianV1")
				.def("apply", &MixtureOfGaussianV1::apply)
				.def("getBackgroundModel", &MixtureOfGaussianV1::getBackgroundModel)
				;
#endif

			py::class_<MixtureOfGaussianV2>("MixtureOfGaussianV2")
				.def("apply", &MixtureOfGaussianV2::apply)
				.def("getBackgroundModel", &MixtureOfGaussianV2::getBackgroundModel)
				;

			py::class_<MultiCue>("MultiCue")
				.def("apply", &MultiCue::apply)
				.def("getBackgroundModel", &MultiCue::getBackgroundModel)
				;

			py::class_<MultiLayer>("MultiLayer")
				.def("apply", &MultiLayer::apply)
				.def("getBackgroundModel", &MultiLayer::getBackgroundModel)
				;

			py::class_<PAWCS>("PAWCS")
				.def("apply", &PAWCS::apply)
				.def("getBackgroundModel", &PAWCS::getBackgroundModel)
				;

			py::class_<PixelBasedAdaptiveSegmenter>("PixelBasedAdaptiveSegmenter")
				.def("apply", &PixelBasedAdaptiveSegmenter::apply)
				.def("getBackgroundModel", &PixelBasedAdaptiveSegmenter::getBackgroundModel)
				;

			py::class_<SigmaDelta>("SigmaDelta")
				.def("apply", &SigmaDelta::apply)
				.def("getBackgroundModel", &SigmaDelta::getBackgroundModel)
				;

			py::class_<SuBSENSE>("SuBSENSE")
				.def("apply", &SuBSENSE::apply)
				.def("getBackgroundModel", &SuBSENSE::getBackgroundModel)
				;

			py::class_<T2FGMM_UM>("T2FGMM_UM")
				.def("apply", &T2FGMM_UM::apply)
				.def("getBackgroundModel", &T2FGMM_UM::getBackgroundModel)
				;

			py::class_<T2FGMM_UV>("T2FGMM_UV")
				.def("apply", &T2FGMM_UV::apply)
				.def("getBackgroundModel", &T2FGMM_UV::getBackgroundModel)
				;

			py::class_<T2FMRF_UM>("T2FMRF_UM")
				.def("apply", &T2FMRF_UM::apply)
				.def("getBackgroundModel", &T2FMRF_UM::getBackgroundModel)
				;

			py::class_<T2FMRF_UV>("T2FMRF_UV")
				.def("apply", &T2FMRF_UV::apply)
				.def("getBackgroundModel", &T2FMRF_UV::getBackgroundModel)
				;

			py::class_<VuMeter>("VuMeter")
				.def("apply", &VuMeter::apply)
				.def("getBackgroundModel", &VuMeter::getBackgroundModel)
				;

			py::class_<WeightedMovingMean>("WeightedMovingMean")
				.def("apply", &WeightedMovingMean::apply)
				.def("getBackgroundModel", &WeightedMovingMean::getBackgroundModel)
				;

			py::class_<WeightedMovingVariance>("WeightedMovingVariance")
				.def("apply", &WeightedMovingVariance::apply)
				.def("getBackgroundModel", &WeightedMovingVariance::getBackgroundModel)
				;

			py::class_<TwoPoints>("TwoPoints")
				.def("apply", &TwoPoints::apply)
				.def("getBackgroundModel", &TwoPoints::getBackgroundModel)
				;

			py::class_<ViBe>("ViBe")
				.def("apply", &ViBe::apply)
				.def("getBackgroundModel", &ViBe::getBackgroundModel)
				;

			py::class_<CodeBook>("CodeBook")
				.def("apply", &CodeBook::apply)
				.def("getBackgroundModel", &CodeBook::getBackgroundModel)
				;
		}
	} // namespace fs
} // namespace python

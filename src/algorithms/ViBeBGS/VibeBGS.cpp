#include "VibeBGS.hpp"

#include <iostream>
#include <execution>

namespace bgslibrary
{
	namespace algorithms
	{
        namespace sky360 {

            VibeBGS::VibeBGS(size_t _nColorDistThreshold,
                        size_t _nBGSamples,
                        size_t _nRequiredBGSamples,
                        size_t _learningRate)
                : m_params(_nColorDistThreshold, _nBGSamples, _nRequiredBGSamples, _learningRate)
            {}

            void VibeBGS::initialize(const Img& _initImg) {
                initialize(_initImg, m_bgImgSamples, m_bgImgPtrSamples);
            }

            void VibeBGS::initialize(const cv::Mat& _initImg) {
                Img frameImg(_initImg.data, ImgSize(_initImg.size().width, _initImg.size().height, 3));
                initialize(frameImg, m_bgImgSamples, m_bgImgPtrSamples);
            }

            void VibeBGS::initializeParallel(const Img& _initImg, int _numProcesses) {
                m_numProcessesParallel = _numProcesses;
                std::vector<std::shared_ptr<Img>> imgSplit(_numProcesses);
                splitImg(_initImg, imgSplit, _numProcesses);

                m_processSeq.resize(_numProcesses);
                m_bgImgSamplesParallel.resize(_numProcesses);
                m_bgImgPtrSamplesParallel.resize(_numProcesses);

                for (int i{0}; i < _numProcesses; ++i) {
                    m_processSeq[i] = i;
                    initialize(*imgSplit[i], m_bgImgSamplesParallel[i], m_bgImgPtrSamplesParallel[i]);
                }
            }

            void VibeBGS::initializeParallel(const cv::Mat& _initImg, int _numProcesses) {
                Img frameImg(_initImg.data, ImgSize(_initImg.size().width, _initImg.size().height, 3));
                initializeParallel(frameImg, _numProcesses);
            }

            void VibeBGS::initialize(const Img& _initImg, std::vector<std::unique_ptr<Img>>& _bgImgSamples, std::vector<Img*>& _bgImgPtrSamples) {
                int ySample, xSample;
                _bgImgSamples.resize(m_params.NBGSamples);
                _bgImgPtrSamples.resize(m_params.NBGSamples);
                for (size_t s = 0; s < m_params.NBGSamples; ++s) {
                    _bgImgSamples[s] = Img::create(_initImg.size, false);
                    _bgImgPtrSamples[s] = _bgImgSamples[s].get();
                    for (int yOrig = 0; yOrig < _initImg.size.height; yOrig++) {
                        for (int xOrig = 0; xOrig < _initImg.size.width; xOrig++) {
                            getSamplePosition_7x7_std2(Pcg32::fast(), xSample, ySample, xOrig, yOrig, _initImg.size);
                            const size_t pixelPos = (yOrig * _initImg.size.width + xOrig) * _initImg.size.numBytesPerPixel;
                            const size_t samplePos = (ySample * _initImg.size.width + xSample) * _initImg.size.numBytesPerPixel;
                            _bgImgSamples[s]->data[pixelPos] = _initImg.data[samplePos];
                            _bgImgSamples[s]->data[pixelPos + 1] = _initImg.data[samplePos + 1];
                            _bgImgSamples[s]->data[pixelPos + 2] = _initImg.data[samplePos + 2];
                        }
                    }
                }
            }

            void VibeBGS::apply(const Img& _image, Img& _fgmask) {
                apply(_image, m_bgImgPtrSamples, _fgmask, m_params);
            }

            void VibeBGS::apply(const cv::Mat& _image, cv::Mat& _fgmask) {
                Img applyImg(_image.data, ImgSize(_image.size().width, _image.size().height, 3));
                Img maskImg(_fgmask.data, ImgSize(_fgmask.size().width, _fgmask.size().height, 1));
                apply(applyImg, m_bgImgPtrSamples, maskImg, m_params);
            }

            void VibeBGS::applyParallel(const Img& _inputImg, Img& _fgmask) {
                std::for_each(
                    std::execution::par_unseq,
                    m_processSeq.begin(),
                    m_processSeq.end(),
                    [&](int np)
                    {
                        // std::cout << "np: " << np << ", pos: " << m_bgImgSamplesParallel[np][0]->size.originalPixelPos 
                        //         << ", size: " << m_bgImgSamplesParallel[np][0]->size.width << ", "
                        //         << m_bgImgSamplesParallel[np][0]->size.height << std::endl;
                        Img imgSplit(_inputImg.data + (m_bgImgSamplesParallel[np][0]->size.originalPixelPos * 3), 
                                    ImgSize(m_bgImgSamplesParallel[np][0]->size.width, m_bgImgSamplesParallel[np][0]->size.height, _inputImg.size.numBytesPerPixel));
                        Img maskPartial(_fgmask.data + m_bgImgSamplesParallel[np][0]->size.originalPixelPos, 
                                    ImgSize(imgSplit.size.width, imgSplit.size.height, 1));
                        VibeBGS::apply(imgSplit, m_bgImgPtrSamplesParallel[np], maskPartial, m_params);
                    });
            }

            void VibeBGS::applyParallelJoin(const Img& _inputImg, Img& _fgmask, std::vector<Img*>& bgImg, const Params& _params) {
                Img imgSplit(_inputImg.data + (bgImg[0]->size.originalPixelPos * 3), 
                            ImgSize(bgImg[0]->size.width, bgImg[0]->size.height, _inputImg.size.numBytesPerPixel));
                Img maskPartial(_fgmask.data + bgImg[0]->size.originalPixelPos, 
                            ImgSize(imgSplit.size.width, imgSplit.size.height, 1));
                apply(imgSplit, bgImg, maskPartial, _params);
            }

            void VibeBGS::applyParallelThread(const Img& _inputImg, Img& _fgmask) {
                std::vector<std::thread> threads(m_numProcessesParallel);
        
                for (int np = 0; np < m_numProcessesParallel; ++np) {
                    threads[np] = std::thread(applyParallelJoin, 
                        std::ref(_inputImg),
                        std::ref(_fgmask),
                        std::ref(m_bgImgPtrSamplesParallel[np]),
                        std::ref(m_params));
                }

                for (auto &th : threads) {
                    th.join();
                }
            }

            void VibeBGS::applyParallel(const cv::Mat& _image, cv::Mat& _fgmask) {
                Img applyImg(_image.data, ImgSize(_image.size().width, _image.size().height, 3));
                Img maskImg(_fgmask.data, ImgSize(_fgmask.size().width, _fgmask.size().height, 1));
                applyParallelThread(applyImg, maskImg);
            }

            void VibeBGS::apply(const Img& image, std::vector<Img*>& bgImg, Img& fgmask, const Params& _params) {
                fgmask.clear();

                for (int pixOffset{0}, colorPixOffset{0}; 
                        pixOffset < image.size.numPixels; 
                        ++pixOffset, colorPixOffset += 3) {
                    size_t nGoodSamplesCount{0}, 
                        nSampleIdx{0};

                    const uchar* const pixData{&image.data[colorPixOffset]};

                    while ((nGoodSamplesCount < _params.NRequiredBGSamples) 
                            && (nSampleIdx < _params.NBGSamples)) {
                        const uchar* const bg{&bgImg[nSampleIdx]->data[colorPixOffset]};
                        if (L2dist3Squared(pixData, bg) < _params.NColorDistThresholdSquared) {
                            ++nGoodSamplesCount;
                        }
                        ++nSampleIdx;
                    }
                    if (nGoodSamplesCount < _params.NRequiredBGSamples) {
                        fgmask.data[pixOffset] = UCHAR_MAX;
                    } else {
                        uint32_t rand = Pcg32::fast();
                        // if ((Pcg32::fast() % m_learningRate) == 0) {
                        if ((rand & _params.ANDlearningRate) == 0) {
                            uchar* const bgImgPixData{&bgImg[rand & _params.ANDlearningRate]->data[colorPixOffset]};
                            bgImgPixData[0] = pixData[0];
                            bgImgPixData[1] = pixData[1];
                            bgImgPixData[2] = pixData[2];
                        }
                        rand = Pcg32::fast();
                        if ((rand & _params.ANDlearningRate) == 0) {
                            int neighData{getNeighborPosition_3x3(pixOffset, image.size)};
                            uchar* const xyRandData{&bgImg[rand & _params.ANDlearningRate]->data[neighData * 3]};
                            xyRandData[0] = pixData[0];
                            xyRandData[1] = pixData[1];
                            xyRandData[2] = pixData[2];
                        }
                    }
                }
            }
        }
   }
}
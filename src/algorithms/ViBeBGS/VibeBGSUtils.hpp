#pragma once

#include "pcg32.hpp"

#include <array>
#include <memory>
#include <vector>

namespace bgslibrary
{
	namespace algorithms
	{
        namespace sky360 {
            typedef unsigned char uchar;
        
            // Todo: This should NOT update the properties manually
            struct ImgSize {
                ImgSize(const ImgSize& _imgSize)
                    : ImgSize(_imgSize.width, _imgSize.height, _imgSize.numBytesPerPixel, _imgSize.originalPixelPos) {
                }

                ImgSize(int _width, int _height, int _numBytesPerPixel, size_t _originalPixelPos = 0) 
                    : width(_width), 
                    height(_height),
                    numBytesPerPixel(_numBytesPerPixel),
                    numPixels(_width * _height),
                    size(_width * _height * _numBytesPerPixel),
                    originalPixelPos{_originalPixelPos}
                {}

                const int width;
                const int height;
                const int numBytesPerPixel;
                const int numPixels;
                const size_t size;

                const size_t originalPixelPos;
            };

            struct Img {
                Img(uchar* _data, const ImgSize& _imgSize, bool _allocated = false)
                    : data{_data},
                    size{_imgSize},
                    allocated{_allocated} {
                }

                ~Img() {
                    if (allocated) {
                        delete[] data;
                    }
                }
                
                static std::unique_ptr<Img> create(const ImgSize& _imgSize, bool _clear = false) {
                    auto data = new uchar[_imgSize.size];
                    if (_clear) {
                        memset(data, 0, _imgSize.size);
                    }

                    return std::make_unique<Img>(data, _imgSize, true);
                }

                inline void clear() {
                    memset(data, 0, size.size);
                }

                const ImgSize size;
                uchar* const data;
                const bool allocated;
            };

            static inline size_t L2dist3Squared(const uchar* const a, const uchar* const b) {
                const long r0{a[0] - b[0]};
                const long r1{a[1] - b[1]};
                const long r2{a[2] - b[2]};
                return (r0 * r0) + (r1 * r1) + (r2 * r2);
            }

            /// returns the neighbor location for the specified random index & original pixel location; also guards against out-of-bounds values via image/border size check
            static inline int getNeighborPosition_3x3(const int pix, const ImgSize& oImageSize, Pcg32& pgc32) {
                typedef std::array<int, 2> Nb;
                static const std::array<std::array<int, 2>, 8> s_anNeighborPattern = {
                        Nb{-1, 1},Nb{0, 1},Nb{1, 1},
                        Nb{-1, 0},         Nb{1, 0},
                        Nb{-1,-1},Nb{0,-1},Nb{1,-1},
                };
                const size_t r{pgc32.fast() & 0x7};
                int nNeighborCoord_X{std::max(std::min((pix % oImageSize.width) + s_anNeighborPattern[r][0], oImageSize.width - 1), 0)};
                int nNeighborCoord_Y{std::max(std::min((pix / oImageSize.width) + s_anNeighborPattern[r][1], oImageSize.height - 1), 0)};
                return (nNeighborCoord_Y * oImageSize.width + nNeighborCoord_X) * 3;
            }

            /// returns pixel coordinates clamped to the given image & border size
            inline void clampImageCoords(int& nSampleCoord_X, int& nSampleCoord_Y, const ImgSize& oImageSize) {
                if (nSampleCoord_X < 0)
                    nSampleCoord_X = 0;
                else if (nSampleCoord_X >= oImageSize.width)
                    nSampleCoord_X = oImageSize.width - 1;
                if (nSampleCoord_Y < 0)
                    nSampleCoord_Y = 0;
                else if (nSampleCoord_Y >= oImageSize.height)
                    nSampleCoord_Y = oImageSize.height - 1;
            }

            /// returns the sampling location for the specified random index & original pixel location, given a predefined kernel; also guards against out-of-bounds values via image/border size check
            template<int nKernelHeight, int nKernelWidth>
            inline void getSamplePosition(const std::array<std::array<int, nKernelWidth>, nKernelHeight>& anSamplesInitPattern,
                const int nSamplesInitPatternTot, const int nRandIdx, int& nSampleCoord_X, int& nSampleCoord_Y,
                const int nOrigCoord_X, const int nOrigCoord_Y, const ImgSize& oImageSize) {
                int r = 1 + (nRandIdx % nSamplesInitPatternTot);
                for (nSampleCoord_Y = 0; nSampleCoord_Y < nKernelHeight; ++nSampleCoord_Y) {
                    for (nSampleCoord_X = 0; nSampleCoord_X < nKernelWidth; ++nSampleCoord_X) {
                        r -= anSamplesInitPattern[nSampleCoord_Y][nSampleCoord_X];
                        if (r <= 0)
                            goto stop;
                    }
                }
            stop:
                nSampleCoord_X += nOrigCoord_X - nKernelWidth / 2;
                nSampleCoord_Y += nOrigCoord_Y - nKernelHeight / 2;
                clampImageCoords(nSampleCoord_X, nSampleCoord_Y, oImageSize);
            }

            /// returns the sampling location for the specified random index & original pixel location; also guards against out-of-bounds values via image/border size check
            inline void getSamplePosition_7x7_std2(const int nRandIdx, 
                int& nSampleCoord_X, int& nSampleCoord_Y,
                const int nOrigCoord_X, const int nOrigCoord_Y,
                const ImgSize& oImageSize) {
                // based on 'floor(fspecial('gaussian',7,2)*512)'
                static const int s_nSamplesInitPatternTot = 512;
                static const std::array<std::array<int, 7>, 7> s_anSamplesInitPattern = {
                        std::array<int,7>{ 2, 4, 6, 7, 6, 4, 2,},
                        std::array<int,7>{ 4, 8,12,14,12, 8, 4,},
                        std::array<int,7>{ 6,12,21,25,21,12, 6,},
                        std::array<int,7>{ 7,14,25,28,25,14, 7,},
                        std::array<int,7>{ 6,12,21,25,21,12, 6,},
                        std::array<int,7>{ 4, 8,12,14,12, 8, 4,},
                        std::array<int,7>{ 2, 4, 6, 7, 6, 4, 2,},
                };
                getSamplePosition<7, 7>(s_anSamplesInitPattern, s_nSamplesInitPatternTot, nRandIdx, nSampleCoord_X, nSampleCoord_Y, nOrigCoord_X, nOrigCoord_Y, oImageSize);
            }

            static inline void splitImg(const Img& _inputImg, std::vector<std::shared_ptr<Img>>& _outputImages, int _numSplits) {
                _outputImages.resize(_numSplits);
                int y = 0;
                int h = _inputImg.size.height / _numSplits;
                for (int i = 0; i < _numSplits; ++i) {
                    if (i == (_numSplits - 1)) {
                        h = _inputImg.size.height - y;
                    }
                    _outputImages[i] = Img::create(ImgSize(_inputImg.size.width, h, _inputImg.size.numBytesPerPixel, y * _inputImg.size.width), false);
                    memcpy(_outputImages[i]->data, 
                        _inputImg.data + (_outputImages[i]->size.originalPixelPos * _inputImg.size.numBytesPerPixel), 
                        _outputImages[i]->size.size);
                    y += h;
                }
            }

            struct Params {
                    Params(size_t nColorDistThreshold,
                        size_t nBGSamples,
                        size_t nRequiredBGSamples,
                        size_t learningRate) 
                        : NBGSamples(nBGSamples),
                        NRequiredBGSamples(nRequiredBGSamples),
                        NColorDistThreshold(nColorDistThreshold),
                        NColorDistThresholdSquared{(nColorDistThreshold * 3) * (nColorDistThreshold * 3)},
                        LearningRate(learningRate),
                        ANDlearningRate{learningRate - 1}
                    {}

                    /// number of different samples per pixel/block to be taken from input frames to build the background model ('N' in the original ViBe paper)
                    const size_t NBGSamples;
                    /// number of similar samples needed to consider the current pixel/block as 'background' ('#_min' in the original ViBe paper)
                    const size_t NRequiredBGSamples;
                    /// absolute color distance threshold ('R' or 'radius' in the original ViBe paper)
                    const size_t NColorDistThreshold;
                    const size_t NColorDistThresholdSquared;
                    /// should be > 0 and factor of 2 (smaller values == faster adaptation)
                    const size_t LearningRate;
                    const size_t ANDlearningRate;
            };
        } 
    }
}
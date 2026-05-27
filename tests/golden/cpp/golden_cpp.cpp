// Golden-master C++ emitter for BGSLibrary (Phase 0 safety net; see MODERNIZATION_ROADMAP.md
// and tests/golden/README.md). Drives algorithms through the core BGS_Factory and prints
// per-frame foreground-mask hashes in the SAME token format as the Python harness
// (run_algorithm.py), so the C++ goldens are directly cross-checkable against the Python ones.
//
// One algorithm per process invocation (driven by tests/golden/golden_cpp.py), mirroring the
// Python subprocess-per-algorithm isolation: each run starts from a clean global rand() seed
// and a fresh ./config (the driver runs us in a temp cwd), and a hard crash in one algorithm's
// teardown (e.g. the known VuMeter munmap_chunk) cannot take the others down. The JSON is
// printed and flushed BEFORE the algorithm object is destroyed, so a destructor crash is
// harmless once a result has been emitted.
//
// Usage:
//   golden_cpp list                            -> prints registered algorithm names, one per line
//   golden_cpp run <NAME> <framesDir> <outFile>-> writes {"algorithm":...,"status":...,"frames":[...]}
//                                                  to <outFile> (NOT stdout: some algorithms print to
//                                                  stdout, which would corrupt a parse)
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "../../../bgslibrary/algorithms/algorithms.h"

using namespace bgslibrary::algorithms;

// ----------------------------------------------------------------------------- SHA-256
// Minimal FIPS 180-4 SHA-256 (public-domain style). Must match Python hashlib.sha256 so the
// C++ tokens equal the Python ones; the full cross-check validates this at scale.
namespace {
struct Sha256 {
  uint32_t h[8];
  uint64_t len = 0;
  uint8_t buf[64];
  size_t buflen = 0;
  static uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
  Sha256() {
    static const uint32_t init[8] = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
                                     0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    std::memcpy(h, init, sizeof(init));
  }
  void block(const uint8_t* p) {
    static const uint32_t k[64] = {
      0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
      0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
      0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
      0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
      0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
      0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
      0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
      0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};
    uint32_t w[64];
    for (int i = 0; i < 16; ++i)
      w[i] = (uint32_t(p[i*4]) << 24) | (uint32_t(p[i*4+1]) << 16) | (uint32_t(p[i*4+2]) << 8) | uint32_t(p[i*4+3]);
    for (int i = 16; i < 64; ++i) {
      uint32_t s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
      uint32_t s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
      w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
    for (int i = 0; i < 64; ++i) {
      uint32_t S1 = rotr(e,6) ^ rotr(e,11) ^ rotr(e,25);
      uint32_t ch = (e & f) ^ (~e & g);
      uint32_t t1 = hh + S1 + ch + k[i] + w[i];
      uint32_t S0 = rotr(a,2) ^ rotr(a,13) ^ rotr(a,22);
      uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
      uint32_t t2 = S0 + maj;
      hh=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
  }
  void update(const uint8_t* data, size_t n) {
    len += n;
    while (n) {
      size_t take = std::min(n, size_t(64) - buflen);
      std::memcpy(buf + buflen, data, take);
      buflen += take; data += take; n -= take;
      if (buflen == 64) { block(buf); buflen = 0; }
    }
  }
  std::string hexdigest() {
    uint64_t bits = len * 8;
    uint8_t pad = 0x80;
    update(&pad, 1);
    uint8_t zero = 0;
    while (buflen != 56) update(&zero, 1);
    uint8_t lenbe[8];
    for (int i = 0; i < 8; ++i) lenbe[i] = uint8_t(bits >> (56 - i*8));
    // append length without counting it into len (update() would, but we are finalizing)
    for (int i = 0; i < 8; ++i) { buf[buflen++] = lenbe[i]; }
    block(buf);
    static const char* hx = "0123456789abcdef";
    std::string out;
    out.reserve(64);
    for (int i = 0; i < 8; ++i)
      for (int s = 28; s >= 0; s -= 4) out.push_back(hx[(h[i] >> s) & 0xF]);
    return out;
  }
};
} // namespace

// numpy dtype string for an OpenCV depth (masks are CV_8U -> "uint8").
static std::string dtype_of(int depth) {
  switch (depth) {
    case CV_8U:  return "uint8";
    case CV_8S:  return "int8";
    case CV_16U: return "uint16";
    case CV_16S: return "int16";
    case CV_32S: return "int32";
    case CV_32F: return "float32";
    case CV_64F: return "float64";
    default:     return "depth" + std::to_string(depth);
  }
}

static std::string basename_of(const std::string& path) {
  size_t s = path.find_last_of("/\\");
  return s == std::string::npos ? path : path.substr(s + 1);
}

static int stem_int(const std::string& path) {
  std::string b = basename_of(path);
  return std::atoi(b.c_str());
}

// Whether the filename stem (basename without extension) is all digits — mirrors the
// Python harness's numeric-stem filter so both levels see the exact same frame set.
static bool stem_is_digits(const std::string& path) {
  std::string b = basename_of(path);
  size_t dot = b.find_last_of('.');
  std::string stem = (dot == std::string::npos) ? b : b.substr(0, dot);
  if (stem.empty()) return false;
  for (char c : stem)
    if (!std::isdigit(static_cast<unsigned char>(c))) return false;
  return true;
}

// "<frame> <WxHxC> <dtype> <sha16>" — matches run_algorithm.py exactly. numpy drops the channel
// axis for single-channel masks, so emit "HxW" when channels==1, else "HxWxC".
static std::string token(const std::string& frame, const cv::Mat& fg) {
  cv::Mat c = fg.isContinuous() ? fg : fg.clone();
  Sha256 sh;
  sh.update(c.data, c.total() * c.elemSize());
  std::string dims = std::to_string(c.rows) + "x" + std::to_string(c.cols);
  if (c.channels() != 1) dims += "x" + std::to_string(c.channels());
  return frame + " " + dims + " " + dtype_of(c.depth()) + " " + sh.hexdigest().substr(0, 16);
}

static std::vector<cv::String> sorted_frames(const std::string& dir) {
  std::vector<cv::String> files;
  cv::glob(dir + "/*.png", files, false);
  files.erase(std::remove_if(files.begin(), files.end(),
              [](const cv::String& f) { return !stem_is_digits(f); }), files.end());
  std::sort(files.begin(), files.end(),
            [](const cv::String& a, const cv::String& b) { return stem_int(a) < stem_int(b); });
  return files;
}

// JSON escape for the error string only (frame tokens and names contain no special chars).
static std::string json_escape(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (c == '"' || c == '\\') { o.push_back('\\'); o.push_back(c); }
    else if (c == '\n' || c == '\r' || c == '\t') o.push_back(' ');
    else o.push_back(c);
  }
  return o;
}

// Write the result JSON to outPath and close it BEFORE the algorithm is destroyed, so the file
// survives a destructor crash (e.g. the known VuMeter munmap_chunk) at process teardown.
static void write_json(const std::string& outPath, const std::string& name, const std::string& status,
                       const std::vector<std::string>& frames, const std::string& error = "") {
  FILE* fp = std::fopen(outPath.c_str(), "w");
  if (!fp) { std::fprintf(stderr, "cannot open %s\n", outPath.c_str()); return; }
  std::fprintf(fp, "{\"algorithm\":\"%s\",\"status\":\"%s\"", name.c_str(), status.c_str());
  if (!error.empty()) std::fprintf(fp, ",\"error\":\"%s\"", json_escape(error).c_str());
  std::fprintf(fp, ",\"frames\":[");
  for (size_t i = 0; i < frames.size(); ++i)
    std::fprintf(fp, "%s\"%s\"", i ? "," : "", frames[i].c_str());
  std::fprintf(fp, "]}\n");
  std::fflush(fp);
  std::fclose(fp);  // data is now in the OS; safe against a subsequent process crash
}

int main(int argc, char** argv) {
  std::string mode = argc > 1 ? argv[1] : "";

  if (mode == "list") {
    auto names = BGS_Factory::Instance()->GetRegisteredAlgorithmsName();
    std::sort(names.begin(), names.end());
    for (const auto& n : names) std::printf("%s\n", n.c_str());
    return 0;
  }

  if (mode == "run" && argc >= 5) {
    std::string name = argv[2];
    std::string framesDir = argv[3];
    std::string outPath = argv[4];
    std::srand(1);  // mirror a fresh process default seed (matches the Python subprocess path)

    auto frames = sorted_frames(framesDir);
    if (frames.empty()) { write_json(outPath, name, "no-frames", {}); return 2; }

    std::shared_ptr<IBGS> bgs;
    try {
      bgs = BGS_Factory::Instance()->Create(name);
    } catch (const std::exception& e) {
      write_json(outPath, name, "init-error", {}, e.what()); return 4;
    }
    if (!bgs) { write_json(outPath, name, "not-found", {}); return 3; }

    std::vector<std::string> tokens;
    try {
      for (const auto& f : frames) {
        cv::Mat img = cv::imread(f, cv::IMREAD_COLOR);
        if (img.empty())  // missing/corrupt file -> caught below, emits a clean error status
          throw std::runtime_error(std::string("failed to load image: ") + f.c_str());
        cv::Mat fg = bgs->apply(img);
        tokens.push_back(token(basename_of(f), fg));
      }
    } catch (const std::exception& e) {
      write_json(outPath, name, "apply-error", tokens, e.what()); return 6;
    }
    write_json(outPath, name, "ok", tokens);  // written+closed here; bgs destructor runs after
    return 0;
  }

  std::fprintf(stderr, "usage: %s list | run <NAME> <framesDir> <outFile>\n", argv[0]);
  return 64;
}

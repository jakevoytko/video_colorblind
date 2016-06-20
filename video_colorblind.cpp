/**
 * Contains a program that converts every pixel of a video into its colorblind
 * version.
 *
 * Copyright 2016 Jacob Voytko (jakevoytko@gmail.com)
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <opencv2/opencv.hpp>

#include "color/color.hpp"
#include "color/proto.hpp"

namespace po = boost::program_options;

using cv::Mat;
using cv::MatIterator_;
using cv::Vec3b;
using cv::VideoCapture;
using cv::VideoWriter;
using cv::Size;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::flush;
using std::runtime_error;
using std::string;
using video_colorblind::color::Rgb;
using video_colorblind::color::Xyz;

// Config represents the configuration of the program.
struct Config {
  Config(string const *_filename, string const *_outputFile):
      filename(*_filename), outputFile(*_outputFile) {}

  const string filename;
  const string outputFile;
};
typedef boost::shared_ptr<Config> SharedConfig;


// parse_config parses the command line options.
SharedConfig parse_config(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce a help message")
      ("input_file", po::value<string>(), "the input file to process")
      ("output_file", po::value<string>(), "the output file");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << endl;
    throw runtime_error("User asked for help");
  } else if (!vm.count("input_file")) {
    cout << desc << endl;
    throw runtime_error("No input_file provided");
  } else if (!vm.count("output_file")) {
    cout << desc << endl;
    throw runtime_error("No output_file provided");
  }

  return SharedConfig(
      boost::make_shared<Config>(&vm["input_file"].as<string>(),
                                 &vm["output_file"].as<string>()));
}


// run processes every frame of the video.
void run(SharedConfig config) {
  VideoCapture videoCapture(config->filename);

  if (!videoCapture.isOpened()) {
    cerr << "Video capture did not open" << endl;
  }

  cout << "Width: " << videoCapture.get(CV_CAP_PROP_FRAME_WIDTH) << endl
       << "Height: " << videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl
       << "FPS: " << videoCapture.get(CV_CAP_PROP_FPS) << endl
       << "Frame count: " << videoCapture.get(CV_CAP_PROP_FRAME_COUNT) << endl
       << "Format: " << videoCapture.get(CV_CAP_PROP_FORMAT) << endl;

  int frameNumber = 0;

  Mat frame;
  VideoWriter outputVideo;
  outputVideo.open(
      config->outputFile,
      videoCapture.get(CV_CAP_PROP_FOURCC),
      videoCapture.get(CV_CAP_PROP_FPS),
      Size(static_cast<int>(videoCapture.get(CV_CAP_PROP_FRAME_WIDTH)),
           static_cast<int>(videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT))),
      true);

  while (videoCapture.read(frame)) {
    try {
      assert(frame.channels() == 3);
      assert(frame.depth() == CV_8U);

      // TODO: Extract square deviation before editing this function again.
      double squareDeviation = 0;
      int points = 0;
      for (auto it = frame.begin<Vec3b>(); it != frame.end<Vec3b>(); it++) {
        points++;
        // OpenCV gives color information in BGR format.
        uchar b = (*it)[0];
        uchar g = (*it)[1];
        uchar r = (*it)[2];

        Rgb rgb(r, g, b);
        Rgb protoRgb = getProtoColor(rgb);
        Xyz xyz = rgbToXyz(rgb);
        Xyz protoXyz = rgbToXyz(protoRgb);
        double distance = sqrt(pow(xyz.x - protoXyz.x, 2) +
                               pow(xyz.y - protoXyz.y, 2) +
                               pow(xyz.z - protoXyz.z, 2));
        squareDeviation += pow(distance, 2);
        (*it)[0] = protoRgb.b;
        (*it)[1] = protoRgb.g;
        (*it)[2] = protoRgb.r;
      }

      double rmse = sqrt(squareDeviation / points);
      std::ostringstream outputText;
      outputText << "RMSD: " << rmse;
      putText(
          frame,
          outputText.str(),
          cv::Point(25, 25),
          cv::FONT_HERSHEY_COMPLEX_SMALL,
          1.0 /* fontScale */,
          cv::Scalar(255, 255, 255, 255));
      outputVideo.write(frame);

      if (!(frameNumber % 100) && frameNumber != 0) {
        cout << "Frame: " << frameNumber << endl << flush;
        cout << "RMSE: " << rmse << endl;
        return;
      }
    } catch (exception &e) {
      cerr << "Error processing frame #" <<
          frameNumber << ": " << e.what() << endl;
    }
    frameNumber++;
  }
}


int main(int argc, char **argv) {
  try {
    SharedConfig config = parse_config(argc, argv);
    run(config);
  } catch(exception &e) {
    cerr << "Error running program: " << e.what() << endl;
    return -1;
  }

  return 0;
}

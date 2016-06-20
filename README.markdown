`video_colorblind`
------------------

A weekend project that converts video into a colorblind version of the video.

Currently, it only supports protanopia. I am protanopic, so I am able to Q/A the
algorithm effectively compared to the other types of colorblindness. In the
future, I may add other types of colorblindness, but I don't plan to.

For a deeper description of the algorithm I used, please see [a talk I gave on
the subject](http://bitlog.com/post/4/colorvision/Transcript-of-I-am-colorblind-and-you-can-too).

This project is licensed under the MIT license.

I haven't done C++ work professionally in 6 years, so I kept it as simple as
possible. I have also made no effort to profile or optimize the code. There is
room for improvement.


Development setup
-----------------

This project is a C++ project. You need three things:

- A local copy of the OpenCV library
- A local copy of Boost with Boost.Test
- [Bazel](http://bazel.io), Google's OSS build tool

Open $project_root/WORKSPACE, and modify the path to OpenCV and Boost.

Use `bazel build :all` to verify that everything can be found.


Running
-------

`bazel run :video_colorblind -- --input_file $input_file --output_file $output_file`

Note the double-dash between :video_colorblind and --input_file.

This will process any video file that your OpenCV is compiled to handle.


Testing
-------

`bazel test :all_tests`

Unit coverage is limited to the color functions. Modify carefully.

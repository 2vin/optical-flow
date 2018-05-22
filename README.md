# optical-flow
Visualizing feature-based optical flow vectors in realtime camera feed

# Compile
`g++ optical-flow.cpp -o main \`pkg-config --cflags --libs opencv\` -std=c++11`

# Usage
./main 

(Note: Recorded video i.e. "video.avi" will be saved in the current directory)

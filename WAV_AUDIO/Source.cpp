#include <iostream> 
#include <cmath>
#include <fstream>
#define M_PI       3.14159265358979323846   // pi
constexpr auto sampleRate = 44100;
constexpr auto bitDepth = 16;

class SineOscillator
{
	float frequency, amplitude, angle = 0.0f , offset = 0.0f;
public :
	SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp)
	{
		offset = 2 * M_PI * frequency / sampleRate;
	}

	float process()
	{
		//asin (2pif/sr)
		auto sample =  amplitude * sin(angle);
		angle += offset;
		return sample;
	}
};

void writeFile(std::ofstream& file, int value, int size)
{
	file.write(reinterpret_cast<const char*> (&value), size);
}


int main()
{
	int duration = 2;
	std::ofstream audioFile;
	audioFile.open("waveform.wav" , std :: ios:: binary);
	SineOscillator sineOscilator(440, 0.5);

	//header chunk 
	audioFile << "RIFF";
	audioFile << "----";
	audioFile << "WAVE";

	//Format chunk 
	audioFile << "fmt ";
	writeFile(audioFile, 16, 4);//size 
	writeFile(audioFile, 1, 2);//compression code 
	writeFile(audioFile, 1, 2); // number of channels
	writeFile(audioFile, sampleRate, 4); // sample rate
	writeFile(audioFile, sampleRate * bitDepth / 8, 4);//byte rate 
	writeFile(audioFile, bitDepth / 8, 2); //block align 
	writeFile(audioFile, bitDepth, 2);//bit depth 

	//data chunk 
	audioFile << "data";
	audioFile << "----";

	//return current position of the poiter 
	int preAudioPosition = audioFile.tellp();

	auto maxAmplitude = pow(2, bitDepth - 1) - 1;
	for (int i = 0; i < sampleRate * duration; i++)
	{
		auto sample = sineOscilator.process();
		int intSample = static_cast<int> (sample * maxAmplitude);
		writeFile(audioFile, intSample, 2);
	}

	int postAudioPosition = audioFile.tellp();

	audioFile.seekp(preAudioPosition - 4);
	writeFile(audioFile, postAudioPosition - preAudioPosition , 4);

	audioFile.seekp(4, std::ios::beg);
	writeFile(audioFile, postAudioPosition - 8, 4);

	audioFile.close();
	return 0;

}
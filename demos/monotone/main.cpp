/*
SoLoud audio engine
Copyright (c) 2013-2015 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "imgui.h"
#include "soloud_demo_framework.h"

#include "soloud.h"
#include "soloud_monotone.h"
#include "soloud_lofifilter.h"
#include "soloud_biquadresonantfilter.h"
#include "soloud_echofilter.h"
#include "soloud_dcremovalfilter.h"

SoLoud::Soloud gSoloud;
SoLoud::Monotone gMusic;
SoLoud::BiquadResonantFilter gBiquad;
SoLoud::LofiFilter gLofi;
SoLoud::EchoFilter gEcho;
SoLoud::DCRemovalFilter gDCRemoval;
int gMusichandle;

// Entry point
int main(int argc, char *argv[])
{
	DemoInit();

	gMusic.load("audio/Jakim - Aboriginal Derivatives.mon", 10);

	gEcho.setParams(0.2f, 0.5f, 0.05f);
	gBiquad.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 4000, 2);

	gMusic.setLooping(1);	
	gMusic.setFilter(0, &gBiquad);
	gMusic.setFilter(1, &gLofi);
	gMusic.setFilter(2, &gEcho);
	gMusic.setFilter(3, &gDCRemoval);

	gSoloud.init(SoLoud::Soloud::CLIP_ROUNDOFF | SoLoud::Soloud::ENABLE_VISUALIZATION);

	gMusichandle = gSoloud.play(gMusic);


	float filter_param0[4] = { 0, 0, 0, 0 };
	float filter_param1[4] = { 1000, 8000, 0, 0 };
	float filter_param2[4] = { 2, 3,  0, 0 };
	
	// Main loop: loop forever.
	while (1)
	{
		gSoloud.setFilterParameter(gMusichandle, 0, 0, filter_param0[0]);
		gSoloud.setFilterParameter(gMusichandle, 1, 0, filter_param0[1]);
		gSoloud.setFilterParameter(gMusichandle, 2, 0, filter_param0[2]);
		gSoloud.setFilterParameter(gMusichandle, 3, 0, filter_param0[3]);

		gSoloud.setFilterParameter(gMusichandle, 0, 2, filter_param1[0]);
		gSoloud.setFilterParameter(gMusichandle, 0, 3, filter_param2[0]);
		gSoloud.setFilterParameter(gMusichandle, 1, 1, filter_param1[1]);
		gSoloud.setFilterParameter(gMusichandle, 1, 2, filter_param2[1]);

		DemoUpdateStart();

		float *buf = gSoloud.getWave();
		float *fft = gSoloud.calcFFT();

		ONCE(ImGui::SetNextWindowPos(ImVec2(500, 20)));
		ImGui::Begin("Output");
		ImGui::PlotLines("##Wave", buf, 256, 0, "Wave", -1, 1, ImVec2(264,80));
		ImGui::PlotHistogram("##FFT", fft, 256/2, 0, "FFT", 0, 10, ImVec2(264,80),8);
		ImGui::Text("Music volume     : %d%%", (int)floor(gSoloud.getVolume(gMusichandle) * 100));
		ImGui::Text("Active voices    : %d", gSoloud.getActiveVoiceCount());
		ImGui::End();

		ONCE(ImGui::SetNextWindowPos(ImVec2(20, 20)));
		ImGui::Begin("Control");
		ImGui::Text("Biquad filter (lowpass)");
		ImGui::SliderFloat("Wet##4", &filter_param0[0], 0, 1);
		ImGui::SliderFloat("Frequency##4", &filter_param1[0], 0, 8000);
		ImGui::SliderFloat("Resonance##4", &filter_param2[0], 1, 20);
		ImGui::Separator();
		ImGui::Text("Lofi filter");
		ImGui::SliderFloat("Wet##2", &filter_param0[1], 0, 1);
		ImGui::SliderFloat("Rate##2", &filter_param1[1], 1000, 8000);
		ImGui::SliderFloat("Bit depth##2", &filter_param2[1], 0, 8);
		ImGui::Separator();
		ImGui::Text("Echo filter");
		ImGui::SliderFloat("Wet##3", &filter_param0[2], 0, 1);
		ImGui::Separator();
		ImGui::Text("DC removal filter");
		ImGui::SliderFloat("Wet##1", &filter_param0[3], 0, 1);
		ImGui::End();
		DemoUpdateEnd();
	}
	return 0;
}
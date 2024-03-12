% DSB modulation investigation
% Cat VW, March `24

% we're going to try to do this by writing everything ourselves, in a way that
% a microcontroller might be happy with!

clc; clear; close all;
pkg load signal;

%% misc. utilities
function nyq_x_axis(nyq, fs, nticks)
	points = linspace(0, 1, nticks);
	ticks = points.*nyq;
	xlim(ticks([1 end]));
	xticks(ticks);
	xticklabels(round(points.*fs/2));
	xlabel('frequency (Hz)');
end

function plot_fft(s, name, fs)
	nticks = 8;
	S = fft(s);
	nyq = round(length(S)/2);
	S_half = S(1:nyq);

	figure;
	subplot(2, 1, 1);
	plot(20*log10(abs(S_half)));
	grid on;
	title(['dB: ' name]);
	nyq_x_axis(nyq, fs, nticks);

	subplot(2, 1, 2);
	plot(unwrap(angle(S_half)));
	grid on;
	title(['phase: ' name]);
	nyq_x_axis(nyq, fs, nticks);
end

%% parameters
N = 8*1024; % samples
fs = 1e6; % Hz
f_if = 115e3; % Hz
f_mod = 2.5e3; % Hz

%% signal creation
n = 0:(N - 1);
wd_if = 2*pi*f_if/fs;
wd_mod = 2*pi*f_mod/fs;

inter = cos(wd_if*n);
mod = cos(wd_mod*n);
signal_in = inter.*mod;

%% signal demodulation
phase_offset = 1.4; % rad
other_inter = cos(wd_if*n + phase_offset);

Wp = 3e3/(fs/2);
Ws = 4e3/(fs/2);
Rp = 1; % dB ripple
Rs = 30; % dB rejection

[order, Wc_s, Wc_p] = cheb2ord(Wp, Ws, Rp, Rs);
[b, a] = cheby2(order, Rs, Wc_s);

after_mult = signal_in.*other_inter;
filtered = filter(b, a, after_mult);
plot_fft(after_mult, 'after mult', fs);
plot_fft(filtered, 'filtered', fs);

figure;
subplot(2, 1, 1);
plot(after_mult);
title('after mult');
subplot(2, 1, 2);
plot(filtered);
title('filtered');

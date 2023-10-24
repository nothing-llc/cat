% SSB modulation investigation
% Cat VW, October `23

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
N = 2^10; % samples
time = 1e-3; % second
f_env = 4.4e3;
f_env_min = 200; % minimum frequency we care about in the envelope
f_if = 10e3;
f_rf = 100e3;
x = linspace(0, time, N)';

%% dependent parameters
fs = N/time;
[fn_env_min, fn_if] = num2cell(2*time/N*[f_env_min, f_if]){:};

%% functions
cos_sin = @(x) [cos(2*pi*x) sin(2*pi*x)];
triangle = @(x) sawtooth(2*pi*x, .5);

%% IF filter
Wp = fn_if - fn_env_min;
Ws = fn_if + fn_env_min;
Rp = 1; % dB ripple
Rs = 12; % dB rejection
[order, Wc_s, Wc_p] = cheb2ord(Wp, Ws, Rp, Rs);
[b, a] = cheby2(order, Rs, Wc_s);

%% the actual transmission
envelope = triangle(f_env*x);
if_carrier = cos_sin(f_if*x); % signal & its approx. Hilbert transform
rf_carrier = cos_sin(f_rf*x); % I & Q; needed for Hilbert SSB

mod_if = envelope.*if_carrier;
filt_if = filter(b, a, mod_if, [], 1);
mod_rf = filt_if.*rf_carrier;

transmission = sum(mod_rf, 2);

%plot_fft(rf_carrier, 'RF carrier', fs);
plot_fft(mod_if(:, 1), 'modulated IF', fs);
plot_fft(filt_if(:, 1), 'filtered IF', fs);
%plot_fft(mod_rf(:, 1), 'modulated RF', fs);
plot_fft(transmission, 'transmission', fs);

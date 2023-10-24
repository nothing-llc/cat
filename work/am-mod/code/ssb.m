% SSB modulation investigation
% Cat VW, October `23

clc; clear; close all;
pkg load signal;

%% misc. utilities

function plot_fft(s, name)
	S = fft(s);
	half_length = round(length(S)/2);
	S_half = S(1:half_length);

	figure;
	subplot(2, 1, 1);
	plot(20*log10(abs(S_half)));
	grid on;
	title(['dB: ' name]);
	xlim([1 half_length]);

	subplot(2, 1, 2);
	plot(unwrap(angle(S_half)));
	grid on;
	title(['phase: ' name]);
	xlim([1 half_length]);
end

%% parameters
N = 1024;
f_env = 3;
f_env_min = 1; % minimum frequency we care about in the envelope
f_if = 30;
f_rf = 100;
x = linspace(0, 1, N)';

%% dependent parameters
[fn_env, fn_env_min, fn_if, fn_rf] = ...
	num2cell(2/N*[f_env, f_env_min, f_if, f_rf]){:};

%% functions
cos_sin = @(x) [cos(N*pi*x) sin(N*pi*x)];
triangle = @(x) sawtooth(N*pi*x, .5);

%% IF filter
Wp = fn_if - fn_env_min;
Ws = fn_if + fn_env_min;
Rp = 1; % dB ripple
Rs = 3; % dB rejection
[order, Wc_s, Wc_p] = cheb2ord(Wp, Ws, Rp, Rs);
[b, a] = cheby2(order, Rs, Wc_s);

%% the actual transmission
envelope = triangle(fn_env*x);
if_carrier = cos_sin(fn_if*x); % signal & its approx. Hilbert transform
rf_carrier = cos_sin(fn_rf*x); % I & Q; needed for Hilbert SSB

mod_if = envelope.*if_carrier;
filt_if = filter(b, a, mod_if, [], 1);
mod_rf = filt_if.*rf_carrier;

transmission = sum(mod_rf, 2);

%plot_fft(rf_carrier, 'RF carrier');
plot_fft(mod_if(:, 1), 'modulated IF');
plot_fft(filt_if(:, 1), 'filtered IF');
%plot_fft(mod_rf(:, 1), 'modulated RF');
plot_fft(transmission, 'transmission');

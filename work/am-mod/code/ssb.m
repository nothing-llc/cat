% SSB modulation investigation
% Cat VW, October `23

clc; clear; close all;
pkg load signal;

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

% parameters
N = 1024;
w_env = 5;
w_if = 66;
w_rf = 300;
x = linspace(0, 2*pi, N)';

% functions
cos_sin = @(x) [cos(x) sin(x)];
triangle = @(x) sawtooth(x, .5);

envelope = triangle(w_env*x);
if_carrier = cos_sin(w_if*x); % signal & its approx. Hilbert transform
rf_carrier = cos_sin(w_rf*x); % I & Q; needed for Hilbert SSB

mod_if = envelope.*if_carrier;
mod_rf = mod_if.*rf_carrier;

transmission = sum(mod_rf, 2);

plot_fft(mod_if(:, 1), 'modulated IF');
plot_fft(mod_rf(:, 1), 'modulated RF');
plot_fft(transmission, 'transmission');

% SSB modulation investigation
% Cat VW, October `23

clc; clear; close all;

function plot_fft(s)
	S = fft(s);
	half_length = round(length(S)/2);
	S_half = S(1:half_length);

	figure;
	subplot(2, 1, 1);
	plot(abs(S_half));
	title('magnitude');
	xlim([1 half_length]);

	subplot(2, 1, 2);
	plot(unwrap(angle(S_half)));
	title('phase');
	xlim([1 half_length]);
end

% parameters
N = 1024;
w_carrier = 500;
w_mod = 44;

cos_sin = @(w, x) [cos(w*x) sin(w*x)];

x = linspace(0, 2*pi, N)';
carrier = cos_sin(w_carrier, x); % I & Q; needed for Hilbert SSB
signal = cos_sin(w_mod, x); % signal & its Hilbert transform

mod_carrier = carrier.*signal;
transmission = sum(mod_carrier, 2);

plot_fft(mod_carrier(:, 1));
plot_fft(transmission);

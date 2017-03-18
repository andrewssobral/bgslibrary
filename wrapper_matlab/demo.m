%{
/*
 * This file is part of BGSLibrary.
 *
 * BGSLibrary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BGSLibrary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
%}
% Example:
% demo('FrameDifference')
% demo('StaticFrameDifference')
% See the full list of available algorithms in the BGSLibrary website.
function demo(algorithm)
if(nargin < 1)
  algorithm = 'FrameDifference';
end

% Create video reader object
%filename = 'visiontraffic.avi';
filename = '../dataset/demo.avi';
hsrc = vision.VideoFileReader(filename, ...
                              'ImageColorSpace', 'RGB', ...
                              'VideoOutputDataType', 'uint8');

% Create background/foreground segmentation object
hfg = backgroundSubtractor(algorithm);

disp('Starting...');
frameCnt = 1;
while ~isDone(hsrc), %disp(frameCnt)
  % Read frame
  frame = step(hsrc);
  
  % Compute foreground mask and background model
  [fgMask, bgModel] = getForegroundMask(hfg, frame);
  
  % View results
  subplot(1,3,1), imshow(frame,'InitialMagnification','fit');
  subplot(1,3,2), imshow(fgMask,'InitialMagnification','fit');
  subplot(1,3,3), imshow(bgModel,'InitialMagnification','fit');
  pause(0.01);
  
  % Reset background model
  % This step just demonstrates how to use reset method
  if (frameCnt==15)
    %reset(hfg);
    %break;
  end
  
  frameCnt = frameCnt + 1;
end
disp('Finished!');

release(hfg);
release(hsrc);

classdef backgroundSubtractor
  %backgroundSubtractor Wrapper class for BGSLibrary
  %   obj = backgroundSubtractor(algorithm)
  %   creates an object with properties
  %
  %   Properties:
  %   algorithm      - Algorithm name (e.g. "FrameDifference").
  %
  %   fgMask = getForegroundMask(obj, img) computes foreground mask on
  %   input image, img, for the object defined by obj.
  %
  %   reset(obj) resets object.
  %
  %   release(obj) releases object memory.
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  %  Properties
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  properties
    % See the full list of available algorithms in the BGSLibrary website.
    algorithm = 'FrameDifference';
  end
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  %  Public methods
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  methods
    % Constructor
    function obj = backgroundSubtractor(algorithm)
      if(nargin > 0)
        obj.algorithm = algorithm;
      else
        obj.algorithm = 'FrameDifference';
      end
      disp(['Selected algorithm: ' obj.algorithm]);
      params = struct('algorithm', obj.algorithm);
      backgroundSubtractor_wrapper('construct', params);
    end
    
    % Get foreground mask and background model
    function [fgMask, bgModel] = getForegroundMask(~, img)
      [fgMask, bgModel] = backgroundSubtractor_wrapper('compute', img);
    end
    
    % Reset object states
    function reset(obj)
      % Reset the background model with default parameters
      % This is done in two steps. First free the persistent
      % memory and then reconstruct the model with original
      % parameters
      backgroundSubtractor_wrapper('destroy');
      params = struct('algorithm', obj.algorithm);
      backgroundSubtractor_wrapper('construct', params);
    end
    
    % Release object memory
    function release(~)
      % free persistent memory for model
      backgroundSubtractor_wrapper('destroy');
    end
    
  end
end
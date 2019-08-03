package br.com.bgslibrary.entity;

public class Command
{
  private static String base;
  private static boolean useCamera;
  private static int cameraIndex;
  private static boolean useVideo;
  private static String inputFile;
  private static boolean useDataset;
  private static String datasetFolder;
  private static String trainFile;
  private static String testFile;
  private static boolean useForegroundMaskAnalysis;
  private static String imageRefFile;
  private static int compareAt;

  public static void init()
  {
    base = "./";
    useCamera = false;
    cameraIndex = 0;
    useVideo = false;
    inputFile = "";
    useDataset = false;
    datasetFolder = "";
    trainFile = "";
    testFile = "";
    useForegroundMaskAnalysis = false;
    imageRefFile = "";
    compareAt = 0;
  }
  
  public static void setBase(String aBase)
  {
    base = aBase;
  }

  public static void setUseCamera()
  {
    useCamera = true;
    useVideo = false;
    useDataset = false;
  }

  public static void setCameraIndex(int aCameraIndex)
  {
    cameraIndex = aCameraIndex;
  }

  public static void setUseVideo()
  {
    useCamera = false;
    useVideo = true;
    useDataset = false;
  }

  public static void setInputFile(String aInputFile)
  {
    inputFile = aInputFile;
  }

  public static void setUseDataset()
  {
    useCamera = false;
    useVideo = false;
    useDataset = true;
  }

  public static void setDatasetFolder(String aDatasetFolder)
  {
    datasetFolder = aDatasetFolder;
  }

  public static void setTrainFile(String aTrainFile)
  {
    trainFile = aTrainFile;
  }

  public static void setTestFile(String aTestFile)
  {
    testFile = aTestFile;
  }

  public static void setUseForegroundMaskAnalysis(boolean aUseForegroundMaskAnalysis)
  {
    useForegroundMaskAnalysis = aUseForegroundMaskAnalysis;
  }

  public static void setImageRefFile(String aImageRefFile)
  {
    imageRefFile = aImageRefFile;
  }

  public static void setCompareAt(int aCompareAt)
  {
    compareAt = aCompareAt;
  }
  
  public static boolean validade()
  {
    boolean ok = true;
    
    if(useCamera == false && useVideo == false && useDataset == false)
      ok = false;
    
    if(useVideo == true && inputFile.isEmpty())
      ok = false;
    
    if(useDataset == true && (datasetFolder.isEmpty() || trainFile.isEmpty() || testFile.isEmpty()))
      ok = false;
    
    if(useForegroundMaskAnalysis == true && imageRefFile.isEmpty())
      ok = false;
    
    return ok;
  }
  
  public static String getString()
  {
    StringBuilder sb = new StringBuilder();
    
    //if(validade())
    {
      sb.append(base).append("bgslibrary.exe ");
      
      if(useCamera)
        sb.append("--use_cam=true --camera=").append(cameraIndex).append(" ");
      
      if(useVideo)
        sb.append("--use_file=true --filename=").append(inputFile).append(" ");
      
      if(useDataset)
        sb.append("--use_dset=true --dset_dir=").append(datasetFolder).append(" --train=").append(trainFile).append(" --test=").append(testFile).append(" ");
      
      if(useForegroundMaskAnalysis)
        sb.append("--use_comp=true --stopAt=").append(compareAt).append(" --imgref=").append(imageRefFile).append(" ");
    }
    
    return sb.toString();
  }
}

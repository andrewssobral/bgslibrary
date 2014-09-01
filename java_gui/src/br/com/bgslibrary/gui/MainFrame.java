/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
package br.com.bgslibrary.gui;

import br.com.bgslibrary.entity.Command;
import br.com.bgslibrary.entity.Configuration;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.DataInputStream;
import java.io.File;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.io.FileUtils;

public class MainFrame extends javax.swing.JFrame
{
  private String basePath = "./";
  private String configPath = basePath + "config/";
  private int appTabbedPaneSelectedIndex = 0;
  
  public MainFrame()
  {
    initComponents();
    init();
  }

  private void init()
  {
    this.setLocationRelativeTo(getRootPane());
    appTabbedPane.setSelectedIndex(0);
    load();
    
    Command.init();
    Command.setBase(basePath);
    /*
    String architectureOS = System.getProperty("os.arch");
    //System.out.println("architectureOS: " + architectureOS);

    if(architectureOS.equals("amd64"))
      win64RadioButtonActionPerformed(null);
    else
      win32RadioButtonActionPerformed(null);
    */
    useCameraRadioButtonActionPerformed(null);
    autoscrollLabelMouseClicked(null);
    autoclearCheckBox.setSelected(true);

    fileNameLabel.setText("");

    update();
    reload();
  }
  
  private void load()
  {
    try
    {
      PropertiesConfiguration pc = new PropertiesConfiguration();
      pc.load("bgslibrary_gui.properties");
      
      basePath = pc.getString("base.path");
      configPath = basePath + pc.getString("config.path");
      
      String paths[] = pc.getStringArray("run.path");
      loadConfigComboBox.setModel(new DefaultComboBoxModel(paths));
      
      String methods[] = pc.getStringArray("bgs.methods");
      tictocComboBox.setModel(new DefaultComboBoxModel(methods));
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void update()
  {
    if(loadConfigComboBox.getSelectedIndex() > 0)
      cmdTextField.setText(loadConfigComboBox.getSelectedItem().toString());
    else
      cmdTextField.setText(Command.getString());
  }
  private ExternalProcess ep = null;
  private Thread externalProcessThread = null;
  private boolean epRunning = false;

  private void execute()
  {
    if(autoclearCheckBox.isSelected())
      logTextPane.setText("");
    
    stop();
    
    if(Command.validade())
      try
      {
        if(epRunning == false)
        {
          ep = new ExternalProcess();
          externalProcessThread = new Thread(ep);
          externalProcessThread.start();
          epRunning = true;
          appTabbedPane.setSelectedIndex(3);
        }
      }
      catch(Exception ex)
      {
        ex.printStackTrace();
      }
    else
      JOptionPane.showMessageDialog(this, "Check parameters!", "Alert", JOptionPane.ERROR_MESSAGE);
  }

  private void stop()
  {
    if(epRunning == true)
    {
      ep.stop();

      try
      {
        Thread.currentThread().sleep(10);
        externalProcessThread.interrupt();
      }
      catch(Exception ex)
      {
      }

      ep = null;
      externalProcessThread = null;
      epRunning = false;
    }
  }

  public void addLog(String msg)
  {
    if(msg.trim().isEmpty())
      return;

    String txt = logTextPane.getText() + msg + "\r\n";
    logTextPane.setText(txt);
  }

  class ExternalProcess implements Runnable
  {
    private boolean stop = false;

    @Override
    public void run()
    {
      Process p = null;
      String str = "";

      try
      {
        //Process p = Runtime.getRuntime().exec(CommandLine.getString());
        //Runtime.getRuntime().exec("cmd /c start /B " + CommandLine.getString());
        //Runtime.getRuntime().exec("cmd /c start /B ../run_video.bat");
        //Runtime.getRuntime().exec("cmd /C " + loadConfigComboBox.getSelectedItem().toString());

        if(loadConfigComboBox.getSelectedIndex() > 0)
          p = Runtime.getRuntime().exec(loadConfigComboBox.getSelectedItem().toString());
        else
          p = Runtime.getRuntime().exec(Command.getString());

        if(p == null)
          return;

        System.out.println("Process created");
        DataInputStream in = new DataInputStream(p.getInputStream());

        while(true)
        {
          if(in.available() > 1)
          {
            str = in.readLine();
            addLog(str);
            System.out.println(str);
          }

          if(stop == true)
            break;
          
          Thread.currentThread().sleep(10);
        }
      }
      catch(Exception ex)
      {
        addLog(ex.getMessage());
        ex.printStackTrace();
      }
      finally
      {
        p.destroy();
        p = null;
        System.out.println("Process destroyed");
      }
    }

    public void stop()
    {
      stop = true;
    }
  }

  private void reload()
  {
    reloadParam("input_resize_percent", resizeFrameTextField, Configuration.VideoCapture);
    reloadParam("stopAt", stopVideoAtTextField, Configuration.VideoCapture);
    reloadParam("enableFlip", verticalFlipCheckBox, Configuration.VideoCapture);
    reloadParam("showOutput", showOutputCheckBox, Configuration.VideoCapture);
    reloadParam("use_roi", useROICheckBox, Configuration.VideoCapture);
    reloadParam("roi_defined", ROIdefinedCheckBox, Configuration.VideoCapture);
    reloadParam("roi_x0", x0ROITextField, Configuration.VideoCapture);
    reloadParam("roi_x1", x1ROITextField, Configuration.VideoCapture);
    reloadParam("roi_y0", y0ROITextField, Configuration.VideoCapture);
    reloadParam("roi_y1", y1ROITextField, Configuration.VideoCapture);

    ///////////////////////////////////////////////////////////////////////////

    reloadParam("enablePreProcessor", preProcessorCheckBox, Configuration.FrameProcessor);
    reloadParam("tictoc", tictocComboBox, Configuration.FrameProcessor);

    ///////////////////////////////////////////////////////////////////////////

    reloadParam("enableFrameDifferenceBGS", frameDifferenceCheckBox, Configuration.FrameProcessor);
    reloadParam("enableStaticFrameDifferenceBGS", staticFrameDifferenceCheckBox, Configuration.FrameProcessor);
    reloadParam("enableWeightedMovingMeanBGS", weightedMovingMeanCheckBox, Configuration.FrameProcessor);
    reloadParam("enableWeightedMovingVarianceBGS", weightedMovingVarianceCheckBox, Configuration.FrameProcessor);
    reloadParam("enableAdaptiveBackgroundLearning", adaptiveBackgroundLearningCheckBox, Configuration.FrameProcessor);
    reloadParam("enableMixtureOfGaussianV1BGS", openCVMoGV1CheckBox, Configuration.FrameProcessor);
    reloadParam("enableMixtureOfGaussianV2BGS", openCVMoGV2CheckBox, Configuration.FrameProcessor);
    reloadParam("enableGMG", GMGCheckBox, Configuration.FrameProcessor);

    ///////////////////////////////////////////////////////////////////////////

    reloadParam("enableDPMeanBGS", temporalMeanCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPAdaptiveMedianBGS", adaptiveMedianCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPPratiMediodBGS", temporalMedianCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPEigenbackgroundBGS", eigenbackgroundCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPWrenGABGS", gaussianAverageCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPGrimsonGMMBGS", grimsonGMMCheckBox, Configuration.FrameProcessor);
    reloadParam("enableDPZivkovicAGMMBGS", zivkovicGMMCheckBox, Configuration.FrameProcessor);

    ///////////////////////////////////////////////////////////////////////////

    reloadParam("enableLBSimpleGaussian", simpleGaussianCheckBox, Configuration.FrameProcessor);
    reloadParam("enableLBFuzzyGaussian", fuzzyGaussianCheckBox, Configuration.FrameProcessor);
    reloadParam("enableLBMixtureOfGaussians", mixtureOfGaussiansCheckBox, Configuration.FrameProcessor);
    reloadParam("enableLBAdaptiveSOM", adaptiveSOMCheckBox, Configuration.FrameProcessor);
    reloadParam("enableLBFuzzyAdaptiveSOM", fuzzyAdaptiveSOMCheckBox, Configuration.FrameProcessor);
    reloadParam("enableT2FGMM_UM", T2FGMMUMCheckBox, Configuration.FrameProcessor);
    reloadParam("enableT2FGMM_UV", T2FGMMUVCheckBox, Configuration.FrameProcessor);
    reloadParam("enableT2FMRF_UM", T2FMRFUMCheckBox, Configuration.FrameProcessor);
    reloadParam("enableT2FMRF_UV", T2FMRFUVCheckBox, Configuration.FrameProcessor);
    reloadParam("enableFuzzySugenoIntegral", fuzzySugenoIntegralCheckBox, Configuration.FrameProcessor);
    reloadParam("enableFuzzyChoquetIntegral", fuzzyChoquetIntegralCheckBox, Configuration.FrameProcessor);
    reloadParam("enableMultiLayerBGS", multiLayerBGSCheckBox, Configuration.FrameProcessor);
    //reloadParam("enablePBAS", PBASCheckBox, Configuration.FrameProcessor);
    reloadParam("enableVuMeter", VuMeterCheckBox, Configuration.FrameProcessor);

    ///////////////////////////////////////////////////////////////////////////

    reloadParam("enableForegroundMaskAnalysis", foregroundMaskAnalysisCheckBox, Configuration.FrameProcessor);
  }

  private void reloadParam(String pname, javax.swing.JTextField textField, String filePath)
  {
    String p = getParam(pname, filePath);
    textField.setText(p);
  }

  private void reloadParam(String pname, javax.swing.JCheckBox checkBox, String filePath)
  {
    String p = getParam(pname, filePath);
    if(Integer.parseInt(p) == 1)
      checkBox.setSelected(true);
    else
      checkBox.setSelected(false);
  }

  private void reloadParam(String pname, javax.swing.JComboBox comboBox, String filePath)
  {
    String p = getParam(pname, filePath);
    if(p.length() > 2)
      comboBox.setSelectedItem(p);
  }

  private String getParam(String paramName, String filePath)
  {
    //System.out.println("changing " + filePath + ", param name:"+paramName+", value:"+paramValue);
    String content = getFileContent(filePath);
    int i1 = content.indexOf("<" + paramName + ">") + ("<" + paramName + ">").length();
    int i2 = content.indexOf("</" + paramName + ">");
    //System.out.println("i1:"+i1+",i2:"+i2);
    String cvalue = content.substring(i1, i2);
    //System.out.println("cvalue:"+cvalue);
    return cvalue;
  }

  private void changeParam(String pname, javax.swing.JCheckBox checkBox, String filePath)
  {
    String nvalue = (checkBox.isSelected() ? 1 : 0) + "";
    changeParam(pname, nvalue, filePath);
  }

  private void changeParam(String pname, javax.swing.JComboBox comboBox, String filePath)
  {
    if(comboBox.getSelectedIndex() > 0)
    {
      String value = comboBox.getSelectedItem().toString();
      changeParam(pname, value, filePath);
    }
    else
      changeParam(pname, "\"\"", filePath);
  }

  private void changeParam(String pname, String nvalue, String filePath)
  {
    String startParam = "<" + pname + ">";
    String endParam = "</" + pname + ">";

    System.out.println("changing " + filePath + ", param name:" + pname + ", value:" + nvalue);

    String content = getFileContent(filePath);
    if(content.isEmpty())
      return;

    int i1 = content.indexOf(startParam) + (startParam).length();
    int i2 = content.indexOf(endParam);
    String cvalue = content.substring(i1, i2);

    //System.out.println("i1:"+i1+",i2:"+i2);
    //System.out.println("cvalue:"+cvalue);

    String cparam = startParam + cvalue + endParam;
    String nparam = startParam + nvalue + endParam;

    String ncontent = content.replace(cparam, nparam);
    //System.out.println("ncontent:"+ncontent);

    try
    {
      File file = new File(configPath + filePath);
      FileUtils.write(file, ncontent);
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private String getFilePath(boolean onlyDir)
  {
    final JFileChooser fc = new JFileChooser(basePath);

    if(onlyDir)
      fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

    int returnVal = fc.showOpenDialog(this);

    String relative = "";

    if(returnVal == JFileChooser.APPROVE_OPTION)
    {
      File file = fc.getSelectedFile();

      String path = file.getAbsolutePath();
      String base = basePath;
      relative = new File(base).toURI().relativize(new File(path).toURI()).getPath();

      System.out.println("Opening: " + relative);
    }
    else
      System.out.println("Open command cancelled by user");

    return relative;
  }

  private String getFileContent(String filePath)
  {
    if(isConfigFile(filePath))
      filePath = configPath + filePath;

    File file = new File(filePath);

    try
    {
      return FileUtils.readFileToString(file);
    }
    catch(java.io.FileNotFoundException ex)
    {
      JOptionPane.showMessageDialog(this, "Configuration file not found.\nFirst enable module and click to run!", "Error", JOptionPane.ERROR_MESSAGE);
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }

    return "";
  }

  private void openFileContent(String filePath)
  {
    fileNameLabel.setText("");

    String content = getFileContent(filePath);

    if(!content.isEmpty())
    {
      fileContentTextPane.setText(content);
      appTabbedPane.setSelectedIndex(2);
      fileNameLabel.setText(filePath);
    }
  }

  private void save()
  {
    String filePath = fileNameLabel.getText();
    String fileContent = fileContentTextPane.getText();

    if(!filePath.isEmpty())
    {
      if(isConfigFile(filePath))
        filePath = configPath + filePath;

      File file = new File(filePath);

      try
      {
        FileUtils.writeStringToFile(file, fileContent);
        //returnButtonActionPerformed(null);
      }
      catch(Exception ex)
      {
        ex.printStackTrace();
      }
    }
  }

  private void retur(boolean clear)
  {
    int index = appTabbedPaneSelectedIndex;
    
    if(clear)
    {
      fileNameLabel.setText("");
      fileContentTextPane.setText("");
      index = 1;
    }

    appTabbedPane.setSelectedIndex(index);
  }

  private boolean isConfigFile(String filename)
  {
    if(filename.endsWith(".xml"))
      return true;
    else
      return false;
  }
  
  AdjustmentListener adjustmentListener = null;
  
  private void autoscroll()
  {
    if(autoscrollCheckBox.isSelected())
    {
      adjustmentListener = new AdjustmentListener()
      {
        @Override
        public void adjustmentValueChanged(AdjustmentEvent e)
        {
          e.getAdjustable().setValue(e.getAdjustable().getMaximum());
        }
      };
      
      logTextScrollPane.getVerticalScrollBar().addAdjustmentListener(adjustmentListener);
    }
    else
      logTextScrollPane.getVerticalScrollBar().removeAdjustmentListener(adjustmentListener);
  }
  
  private void enableAllBGS()
  {
    changeParam("enableFrameDifferenceBGS", "1", Configuration.FrameProcessor);
    changeParam("enableStaticFrameDifferenceBGS", "1", Configuration.FrameProcessor);
    changeParam("enableWeightedMovingMeanBGS", "1", Configuration.FrameProcessor);
    changeParam("enableWeightedMovingVarianceBGS", "1", Configuration.FrameProcessor);
    changeParam("enableAdaptiveBackgroundLearning", "1", Configuration.FrameProcessor);
    changeParam("enableMixtureOfGaussianV1BGS", "1", Configuration.FrameProcessor);
    changeParam("enableMixtureOfGaussianV2BGS", "1", Configuration.FrameProcessor);
    changeParam("enableGMG", "1", Configuration.FrameProcessor);
    changeParam("enableDPMeanBGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPAdaptiveMedianBGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPPratiMediodBGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPEigenbackgroundBGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPWrenGABGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPGrimsonGMMBGS", "1", Configuration.FrameProcessor);
    changeParam("enableDPZivkovicAGMMBGS", "1", Configuration.FrameProcessor);
    changeParam("enableLBSimpleGaussian", "1", Configuration.FrameProcessor);
    changeParam("enableLBFuzzyGaussian", "1", Configuration.FrameProcessor);
    changeParam("enableLBMixtureOfGaussians", "1", Configuration.FrameProcessor);
    changeParam("enableLBAdaptiveSOM", "1", Configuration.FrameProcessor);
    changeParam("enableLBFuzzyAdaptiveSOM", "1", Configuration.FrameProcessor);
    changeParam("enableT2FGMM_UM", "1", Configuration.FrameProcessor);
    changeParam("enableT2FGMM_UV", "1", Configuration.FrameProcessor);
    changeParam("enableT2FMRF_UM", "1", Configuration.FrameProcessor);
    changeParam("enableT2FMRF_UV", "1", Configuration.FrameProcessor);
    changeParam("enableFuzzySugenoIntegral", "1", Configuration.FrameProcessor);
    changeParam("enableFuzzyChoquetIntegral", "1", Configuration.FrameProcessor);
    changeParam("enableMultiLayerBGS", "1", Configuration.FrameProcessor);
    //changeParam("enablePBAS", "1", Configuration.FrameProcessor);
    changeParam("enableVuMeter", "1", Configuration.FrameProcessor);
    
    frameDifferenceCheckBox.setSelected(true);
    staticFrameDifferenceCheckBox.setSelected(true);
    weightedMovingMeanCheckBox.setSelected(true);
    weightedMovingVarianceCheckBox.setSelected(true);
    adaptiveBackgroundLearningCheckBox.setSelected(true);
    openCVMoGV1CheckBox.setSelected(true);
    openCVMoGV2CheckBox.setSelected(true);
    GMGCheckBox.setSelected(true);
    temporalMeanCheckBox.setSelected(true);
    adaptiveMedianCheckBox.setSelected(true);
    temporalMedianCheckBox.setSelected(true);
    eigenbackgroundCheckBox.setSelected(true);
    gaussianAverageCheckBox.setSelected(true);
    grimsonGMMCheckBox.setSelected(true);
    zivkovicGMMCheckBox.setSelected(true);
    simpleGaussianCheckBox.setSelected(true);
    fuzzyGaussianCheckBox.setSelected(true);
    mixtureOfGaussiansCheckBox.setSelected(true);
    adaptiveSOMCheckBox.setSelected(true);
    fuzzyAdaptiveSOMCheckBox.setSelected(true);
    T2FGMMUMCheckBox.setSelected(true);
    T2FGMMUVCheckBox.setSelected(true);
    T2FMRFUMCheckBox.setSelected(true);
    T2FMRFUVCheckBox.setSelected(true);
    fuzzySugenoIntegralCheckBox.setSelected(true);
    fuzzyChoquetIntegralCheckBox.setSelected(true);
    multiLayerBGSCheckBox.setSelected(true);
    //PBASCheckBox.setSelected(true);
    VuMeterCheckBox.setSelected(true);
    
    reload();
  }
  
  private void disableAllBGS()
  {
    changeParam("enableFrameDifferenceBGS", "0", Configuration.FrameProcessor);
    changeParam("enableStaticFrameDifferenceBGS", "0", Configuration.FrameProcessor);
    changeParam("enableWeightedMovingMeanBGS", "0", Configuration.FrameProcessor);
    changeParam("enableWeightedMovingVarianceBGS", "0", Configuration.FrameProcessor);
    changeParam("enableAdaptiveBackgroundLearning", "0", Configuration.FrameProcessor);
    changeParam("enableMixtureOfGaussianV1BGS", "0", Configuration.FrameProcessor);
    changeParam("enableMixtureOfGaussianV2BGS", "0", Configuration.FrameProcessor);
    changeParam("enableGMG", "0", Configuration.FrameProcessor);
    changeParam("enableDPMeanBGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPAdaptiveMedianBGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPPratiMediodBGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPEigenbackgroundBGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPWrenGABGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPGrimsonGMMBGS", "0", Configuration.FrameProcessor);
    changeParam("enableDPZivkovicAGMMBGS", "0", Configuration.FrameProcessor);
    changeParam("enableLBSimpleGaussian", "0", Configuration.FrameProcessor);
    changeParam("enableLBFuzzyGaussian", "0", Configuration.FrameProcessor);
    changeParam("enableLBMixtureOfGaussians", "0", Configuration.FrameProcessor);
    changeParam("enableLBAdaptiveSOM", "0", Configuration.FrameProcessor);
    changeParam("enableLBFuzzyAdaptiveSOM", "0", Configuration.FrameProcessor);
    changeParam("enableT2FGMM_UM", "0", Configuration.FrameProcessor);
    changeParam("enableT2FGMM_UV", "0", Configuration.FrameProcessor);
    changeParam("enableT2FMRF_UM", "0", Configuration.FrameProcessor);
    changeParam("enableT2FMRF_UV", "0", Configuration.FrameProcessor);
    changeParam("enableFuzzySugenoIntegral", "0", Configuration.FrameProcessor);
    changeParam("enableFuzzyChoquetIntegral", "0", Configuration.FrameProcessor);
    changeParam("enableMultiLayerBGS", "0", Configuration.FrameProcessor);
    //changeParam("enablePBAS", "0", Configuration.FrameProcessor);
    changeParam("enableVuMeter", "0", Configuration.FrameProcessor);
    
    frameDifferenceCheckBox.setSelected(false);
    staticFrameDifferenceCheckBox.setSelected(false);
    weightedMovingMeanCheckBox.setSelected(false);
    weightedMovingVarianceCheckBox.setSelected(false);
    adaptiveBackgroundLearningCheckBox.setSelected(false);
    GMGCheckBox.setSelected(false);
    openCVMoGV1CheckBox.setSelected(false);
    openCVMoGV2CheckBox.setSelected(false);
    temporalMeanCheckBox.setSelected(false);
    adaptiveMedianCheckBox.setSelected(false);
    temporalMedianCheckBox.setSelected(false);
    eigenbackgroundCheckBox.setSelected(false);
    gaussianAverageCheckBox.setSelected(false);
    grimsonGMMCheckBox.setSelected(false);
    zivkovicGMMCheckBox.setSelected(false);
    simpleGaussianCheckBox.setSelected(false);
    fuzzyGaussianCheckBox.setSelected(false);
    mixtureOfGaussiansCheckBox.setSelected(false);
    adaptiveSOMCheckBox.setSelected(false);
    fuzzyAdaptiveSOMCheckBox.setSelected(false);
    T2FGMMUMCheckBox.setSelected(false);
    T2FGMMUVCheckBox.setSelected(false);
    T2FMRFUMCheckBox.setSelected(false);
    T2FMRFUVCheckBox.setSelected(false);
    fuzzySugenoIntegralCheckBox.setSelected(false);
    fuzzyChoquetIntegralCheckBox.setSelected(false);
    multiLayerBGSCheckBox.setSelected(false);
    //PBASCheckBox.setSelected(false);
    VuMeterCheckBox.setSelected(false);
    
    reload();
  }

  /**
   * This method is called from within the constructor to initialize the form. WARNING: Do NOT modify this code. The
   * content of this method is always regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents()
  {

    jMenuBar1 = new javax.swing.JMenuBar();
    jMenu1 = new javax.swing.JMenu();
    jMenu2 = new javax.swing.JMenu();
    appTabbedPane = new javax.swing.JTabbedPane();
    jPanel1 = new javax.swing.JPanel();
    jPanel3 = new javax.swing.JPanel();
    useCameraRadioButton = new javax.swing.JRadioButton();
    useVideoRadioButton = new javax.swing.JRadioButton();
    useDatasetRadioButton = new javax.swing.JRadioButton();
    cameraIndexTextField = new javax.swing.JTextField();
    setInputFileButton = new javax.swing.JButton();
    setDatasetFolderButton = new javax.swing.JButton();
    setTrainFileButton = new javax.swing.JButton();
    setTestFileButton = new javax.swing.JButton();
    jLabel2 = new javax.swing.JLabel();
    jPanel4 = new javax.swing.JPanel();
    cmdTextField = new javax.swing.JTextField();
    jPanel6 = new javax.swing.JPanel();
    useForegroundMaskAnalysisCheckBox = new javax.swing.JCheckBox();
    setImageRefButton = new javax.swing.JButton();
    jLabel1 = new javax.swing.JLabel();
    compareAtTextField = new javax.swing.JTextField();
    jPanel7 = new javax.swing.JPanel();
    jLabel3 = new javax.swing.JLabel();
    loadConfigComboBox = new javax.swing.JComboBox();
    editButton = new javax.swing.JButton();
    runButton = new javax.swing.JButton();
    jPanel2 = new javax.swing.JPanel();
    jPanel9 = new javax.swing.JPanel();
    jLabel4 = new javax.swing.JLabel();
    resizeFrameTextField = new javax.swing.JTextField();
    jLabel5 = new javax.swing.JLabel();
    verticalFlipCheckBox = new javax.swing.JCheckBox();
    jLabel6 = new javax.swing.JLabel();
    stopVideoAtTextField = new javax.swing.JTextField();
    showOutputCheckBox = new javax.swing.JCheckBox();
    reloadButton = new javax.swing.JButton();
    jPanel10 = new javax.swing.JPanel();
    useROICheckBox = new javax.swing.JCheckBox();
    ROIdefinedCheckBox = new javax.swing.JCheckBox();
    jLabel7 = new javax.swing.JLabel();
    x0ROITextField = new javax.swing.JTextField();
    jLabel8 = new javax.swing.JLabel();
    y0ROITextField = new javax.swing.JTextField();
    jLabel9 = new javax.swing.JLabel();
    x1ROITextField = new javax.swing.JTextField();
    jLabel10 = new javax.swing.JLabel();
    y1ROITextField = new javax.swing.JTextField();
    jPanel11 = new javax.swing.JPanel();
    jLabel11 = new javax.swing.JLabel();
    jPanel12 = new javax.swing.JPanel();
    preProcessorCheckBox = new javax.swing.JCheckBox();
    tictocComboBox = new javax.swing.JComboBox();
    preProcessorLabel = new javax.swing.JLabel();
    jLabel34 = new javax.swing.JLabel();
    foregroundMaskAnalysisLabel = new javax.swing.JLabel();
    foregroundMaskAnalysisCheckBox = new javax.swing.JCheckBox();
    jPanel13 = new javax.swing.JPanel();
    frameDifferenceCheckBox = new javax.swing.JCheckBox();
    staticFrameDifferenceCheckBox = new javax.swing.JCheckBox();
    weightedMovingMeanCheckBox = new javax.swing.JCheckBox();
    weightedMovingVarianceCheckBox = new javax.swing.JCheckBox();
    openCVMoGV1CheckBox = new javax.swing.JCheckBox();
    openCVMoGV2CheckBox = new javax.swing.JCheckBox();
    adaptiveBackgroundLearningCheckBox = new javax.swing.JCheckBox();
    gaussianAverageCheckBox = new javax.swing.JCheckBox();
    eigenbackgroundCheckBox = new javax.swing.JCheckBox();
    temporalMedianCheckBox = new javax.swing.JCheckBox();
    adaptiveMedianCheckBox = new javax.swing.JCheckBox();
    temporalMeanCheckBox = new javax.swing.JCheckBox();
    grimsonGMMCheckBox = new javax.swing.JCheckBox();
    zivkovicGMMCheckBox = new javax.swing.JCheckBox();
    adaptiveSOMCheckBox = new javax.swing.JCheckBox();
    mixtureOfGaussiansCheckBox = new javax.swing.JCheckBox();
    fuzzyGaussianCheckBox = new javax.swing.JCheckBox();
    simpleGaussianCheckBox = new javax.swing.JCheckBox();
    fuzzyAdaptiveSOMCheckBox = new javax.swing.JCheckBox();
    T2FGMMUMCheckBox = new javax.swing.JCheckBox();
    T2FGMMUVCheckBox = new javax.swing.JCheckBox();
    frameDifferenceLabel = new javax.swing.JLabel();
    staticFrameDifferenceLabel = new javax.swing.JLabel();
    weightedMovingMeanLabel = new javax.swing.JLabel();
    weightedMovingVarianceLabel = new javax.swing.JLabel();
    adaptiveBackgroundLearningLabel = new javax.swing.JLabel();
    openCVMoGV1Label = new javax.swing.JLabel();
    openCVMoGV2Label = new javax.swing.JLabel();
    zivkovicGMMLabel = new javax.swing.JLabel();
    grimsonGMMLabel = new javax.swing.JLabel();
    gaussianAverageLabel = new javax.swing.JLabel();
    eigenbackgroundLabel = new javax.swing.JLabel();
    temporalMedianLabel = new javax.swing.JLabel();
    adaptiveMedianLabel = new javax.swing.JLabel();
    temporalMeanLabel = new javax.swing.JLabel();
    simpleGaussianLabel = new javax.swing.JLabel();
    fuzzyGaussianLabel = new javax.swing.JLabel();
    mixtureOfGaussiansLabel = new javax.swing.JLabel();
    adaptiveSOMLabel = new javax.swing.JLabel();
    fuzzyAdaptiveSOMLabel = new javax.swing.JLabel();
    T2FGMMUMLabel = new javax.swing.JLabel();
    T2FGMMUVLabel = new javax.swing.JLabel();
    multiLayerBGSLabel = new javax.swing.JLabel();
    multiLayerBGSCheckBox = new javax.swing.JCheckBox();
    checkAllButton = new javax.swing.JButton();
    uncheckAllButton = new javax.swing.JButton();
    openConfigButton = new javax.swing.JButton();
    fuzzySugenoIntegralLabel = new javax.swing.JLabel();
    fuzzySugenoIntegralCheckBox = new javax.swing.JCheckBox();
    fuzzyChoquetIntegralLabel = new javax.swing.JLabel();
    fuzzyChoquetIntegralCheckBox = new javax.swing.JCheckBox();
    T2FMRFUMLabel = new javax.swing.JLabel();
    T2FMRFUMCheckBox = new javax.swing.JCheckBox();
    T2FMRFUVLabel = new javax.swing.JLabel();
    T2FMRFUVCheckBox = new javax.swing.JCheckBox();
    GMGCheckBox = new javax.swing.JCheckBox();
    GMGLabel = new javax.swing.JLabel();
    VuMeterLabel = new javax.swing.JLabel();
    VuMeterCheckBox = new javax.swing.JCheckBox();
    runButton2 = new javax.swing.JButton();
    jPanel16 = new javax.swing.JPanel();
    jToolBar1 = new javax.swing.JToolBar();
    saveButton = new javax.swing.JButton();
    jSeparator1 = new javax.swing.JToolBar.Separator();
    returnButton = new javax.swing.JButton();
    jSeparator3 = new javax.swing.JToolBar.Separator();
    runButton3 = new javax.swing.JButton();
    jPanel17 = new javax.swing.JPanel();
    jScrollPane1 = new javax.swing.JScrollPane();
    fileContentTextPane = new javax.swing.JTextPane();
    jLabel12 = new javax.swing.JLabel();
    fileNameLabel = new javax.swing.JLabel();
    jPanel18 = new javax.swing.JPanel();
    logTextScrollPane = new javax.swing.JScrollPane();
    logTextPane = new javax.swing.JTextPane();
    jToolBar2 = new javax.swing.JToolBar();
    clearButton = new javax.swing.JButton();
    jSeparator4 = new javax.swing.JToolBar.Separator();
    runButton4 = new javax.swing.JButton();
    jSeparator2 = new javax.swing.JToolBar.Separator();
    returnButton2 = new javax.swing.JButton();
    jSeparator5 = new javax.swing.JToolBar.Separator();
    autoscrollCheckBox = new javax.swing.JCheckBox();
    autoscrollLabel = new javax.swing.JLabel();
    jSeparator6 = new javax.swing.JToolBar.Separator();
    autoclearCheckBox = new javax.swing.JCheckBox();
    autoclearLabel = new javax.swing.JLabel();
    jMenuBar2 = new javax.swing.JMenuBar();
    jMenu3 = new javax.swing.JMenu();
    jMenuItem1 = new javax.swing.JMenuItem();
    jMenu4 = new javax.swing.JMenu();
    jMenuItem2 = new javax.swing.JMenuItem();

    jMenu1.setText("File");
    jMenuBar1.add(jMenu1);

    jMenu2.setText("Edit");
    jMenuBar1.add(jMenu2);

    setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
    setTitle("BGSLibrary GUI");
    setBackground(new java.awt.Color(255, 255, 255));

    appTabbedPane.setBorder(javax.swing.BorderFactory.createEtchedBorder());

    jPanel3.setBorder(javax.swing.BorderFactory.createTitledBorder("Input"));

    useCameraRadioButton.setText("Use camera");
    useCameraRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        useCameraRadioButtonActionPerformed(evt);
      }
    });

    useVideoRadioButton.setText("Use video file");
    useVideoRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        useVideoRadioButtonActionPerformed(evt);
      }
    });

    useDatasetRadioButton.setText("Use dataset");
    useDatasetRadioButton.setEnabled(false);
    useDatasetRadioButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        useDatasetRadioButtonActionPerformed(evt);
      }
    });

    cameraIndexTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    cameraIndexTextField.setText("0");

    setInputFileButton.setText("Set input file");
    setInputFileButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        setInputFileButtonActionPerformed(evt);
      }
    });

    setDatasetFolderButton.setText("Set dataset folder");
    setDatasetFolderButton.setEnabled(false);
    setDatasetFolderButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        setDatasetFolderButtonActionPerformed(evt);
      }
    });

    setTrainFileButton.setText("Set train file");
    setTrainFileButton.setEnabled(false);
    setTrainFileButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        setTrainFileButtonActionPerformed(evt);
      }
    });

    setTestFileButton.setText("Set test file");
    setTestFileButton.setEnabled(false);
    setTestFileButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        setTestFileButtonActionPerformed(evt);
      }
    });

    jLabel2.setText("Camera index:");

    javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
    jPanel3.setLayout(jPanel3Layout);
    jPanel3Layout.setHorizontalGroup(
      jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel3Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel3Layout.createSequentialGroup()
            .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(useCameraRadioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addComponent(useVideoRadioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel3Layout.createSequentialGroup()
                .addComponent(jLabel2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(cameraIndexTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
              .addComponent(setInputFileButton, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE)))
          .addGroup(jPanel3Layout.createSequentialGroup()
            .addComponent(useDatasetRadioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(setDatasetFolderButton, javax.swing.GroupLayout.PREFERRED_SIZE, 140, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(setTrainFileButton, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(setTestFileButton, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    jPanel3Layout.setVerticalGroup(
      jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel3Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(useCameraRadioButton)
          .addComponent(cameraIndexTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel2))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(useVideoRadioButton)
          .addComponent(setInputFileButton))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(useDatasetRadioButton)
          .addComponent(setDatasetFolderButton)
          .addComponent(setTrainFileButton)
          .addComponent(setTestFileButton))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jPanel4.setBorder(javax.swing.BorderFactory.createTitledBorder("Command Line"));

    javax.swing.GroupLayout jPanel4Layout = new javax.swing.GroupLayout(jPanel4);
    jPanel4.setLayout(jPanel4Layout);
    jPanel4Layout.setHorizontalGroup(
      jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel4Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(cmdTextField)
        .addContainerGap())
    );
    jPanel4Layout.setVerticalGroup(
      jPanel4Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel4Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(cmdTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jPanel6.setBorder(javax.swing.BorderFactory.createTitledBorder("Options"));

    useForegroundMaskAnalysisCheckBox.setText("Use Foreground Mask Analysis");
    useForegroundMaskAnalysisCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        useForegroundMaskAnalysisCheckBoxActionPerformed(evt);
      }
    });

    setImageRefButton.setText("Set Image Ref");
    setImageRefButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        setImageRefButtonActionPerformed(evt);
      }
    });

    jLabel1.setText("Compare at frame:");

    compareAtTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    compareAtTextField.setText("0");
    compareAtTextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        compareAtTextFieldFocusLost(evt);
      }
    });

    javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
    jPanel6.setLayout(jPanel6Layout);
    jPanel6Layout.setHorizontalGroup(
      jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel6Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(useForegroundMaskAnalysisCheckBox)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(setImageRefButton)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jLabel1)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(compareAtTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    jPanel6Layout.setVerticalGroup(
      jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel6Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(useForegroundMaskAnalysisCheckBox)
          .addComponent(setImageRefButton)
          .addComponent(jLabel1)
          .addComponent(compareAtTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jPanel7.setBorder(javax.swing.BorderFactory.createTitledBorder("Saves"));

    jLabel3.setText("Load:");

    loadConfigComboBox.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        loadConfigComboBoxItemStateChanged(evt);
      }
    });

    editButton.setText("Edit");
    editButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        editButtonActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel7Layout = new javax.swing.GroupLayout(jPanel7);
    jPanel7.setLayout(jPanel7Layout);
    jPanel7Layout.setHorizontalGroup(
      jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel7Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jLabel3)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(loadConfigComboBox, 0, 205, Short.MAX_VALUE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(editButton, javax.swing.GroupLayout.PREFERRED_SIZE, 60, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addContainerGap())
    );
    jPanel7Layout.setVerticalGroup(
      jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel7Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel3)
          .addComponent(loadConfigComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(editButton))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    runButton.setText("Run");
    runButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        runButtonActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(jPanel4, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                .addComponent(jPanel6, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(jPanel3, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
              .addComponent(jPanel7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 148, Short.MAX_VALUE)
            .addComponent(runButton, javax.swing.GroupLayout.PREFERRED_SIZE, 80, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap())
    );
    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jPanel4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addComponent(jPanel7, javax.swing.GroupLayout.PREFERRED_SIZE, 65, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jPanel3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jPanel6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addComponent(runButton))
        .addContainerGap(245, Short.MAX_VALUE))
    );

    appTabbedPane.addTab("Main", jPanel1);

    jPanel9.setBorder(javax.swing.BorderFactory.createTitledBorder("Video Capture Module"));

    jLabel4.setText("Resize frame:");

    resizeFrameTextField.setText("100");
    resizeFrameTextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        resizeFrameTextFieldFocusLost(evt);
      }
    });

    jLabel5.setText("%");

    verticalFlipCheckBox.setText("Vertical Flip");
    verticalFlipCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        verticalFlipCheckBoxActionPerformed(evt);
      }
    });

    jLabel6.setText("Stop video at frame:");

    stopVideoAtTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    stopVideoAtTextField.setText("0");
    stopVideoAtTextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        stopVideoAtTextFieldFocusLost(evt);
      }
    });

    showOutputCheckBox.setText("Show output");
    showOutputCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        showOutputCheckBoxActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel9Layout = new javax.swing.GroupLayout(jPanel9);
    jPanel9.setLayout(jPanel9Layout);
    jPanel9Layout.setHorizontalGroup(
      jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel9Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel9Layout.createSequentialGroup()
            .addComponent(jLabel4)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(resizeFrameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jLabel5))
          .addGroup(jPanel9Layout.createSequentialGroup()
            .addComponent(jLabel6)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(stopVideoAtTextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addGroup(jPanel9Layout.createSequentialGroup()
            .addComponent(verticalFlipCheckBox)
            .addGap(18, 18, 18)
            .addComponent(showOutputCheckBox)))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    jPanel9Layout.setVerticalGroup(
      jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel9Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel4)
          .addComponent(resizeFrameTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel5))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel6)
          .addComponent(stopVideoAtTextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(verticalFlipCheckBox)
          .addComponent(showOutputCheckBox))
        .addContainerGap(14, Short.MAX_VALUE))
    );

    reloadButton.setText("Reload");
    reloadButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        reloadButtonActionPerformed(evt);
      }
    });

    jPanel10.setBorder(javax.swing.BorderFactory.createTitledBorder("Region of Interest (ROI)"));

    useROICheckBox.setText("Use ROI");
    useROICheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        useROICheckBoxActionPerformed(evt);
      }
    });

    ROIdefinedCheckBox.setText("ROI defined");
    ROIdefinedCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        ROIdefinedCheckBoxActionPerformed(evt);
      }
    });

    jLabel7.setText("X0:");

    x0ROITextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    x0ROITextField.setText("0");
    x0ROITextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        x0ROITextFieldFocusLost(evt);
      }
    });

    jLabel8.setText("Y0:");

    y0ROITextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    y0ROITextField.setText("0");
    y0ROITextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        y0ROITextFieldFocusLost(evt);
      }
    });

    jLabel9.setText("X1:");

    x1ROITextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    x1ROITextField.setText("0");
    x1ROITextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        x1ROITextFieldFocusLost(evt);
      }
    });

    jLabel10.setText("Y1:");

    y1ROITextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
    y1ROITextField.setText("0");
    y1ROITextField.addFocusListener(new java.awt.event.FocusAdapter()
    {
      public void focusLost(java.awt.event.FocusEvent evt)
      {
        y1ROITextFieldFocusLost(evt);
      }
    });

    javax.swing.GroupLayout jPanel10Layout = new javax.swing.GroupLayout(jPanel10);
    jPanel10.setLayout(jPanel10Layout);
    jPanel10Layout.setHorizontalGroup(
      jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel10Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel10Layout.createSequentialGroup()
            .addComponent(useROICheckBox)
            .addGap(18, 18, 18)
            .addComponent(ROIdefinedCheckBox))
          .addGroup(jPanel10Layout.createSequentialGroup()
            .addComponent(jLabel7)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(x0ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(jLabel9)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(x1ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addGroup(jPanel10Layout.createSequentialGroup()
            .addComponent(jLabel8)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(y0ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(jLabel10)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(y1ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    jPanel10Layout.setVerticalGroup(
      jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel10Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(useROICheckBox)
          .addComponent(ROIdefinedCheckBox))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel7)
          .addComponent(x0ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel9)
          .addComponent(x1ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addGroup(jPanel10Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel8)
          .addComponent(y0ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel10)
          .addComponent(y1ROITextField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    jPanel11.setBorder(javax.swing.BorderFactory.createTitledBorder("Frame Processor Module"));

    jLabel11.setFont(new java.awt.Font("Tahoma", 1, 12)); // NOI18N
    jLabel11.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
    jLabel11.setText("Modules");

    jPanel12.setBorder(javax.swing.BorderFactory.createTitledBorder("Processors"));

    preProcessorCheckBox.setText("PreProcessor");
    preProcessorCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        preProcessorCheckBoxActionPerformed(evt);
      }
    });

    tictocComboBox.addItemListener(new java.awt.event.ItemListener()
    {
      public void itemStateChanged(java.awt.event.ItemEvent evt)
      {
        tictocComboBoxItemStateChanged(evt);
      }
    });

    preProcessorLabel.setForeground(new java.awt.Color(0, 0, 255));
    preProcessorLabel.setText("C");
    preProcessorLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    preProcessorLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        preProcessorLabelMouseClicked(evt);
      }
    });

    jLabel34.setText("Performance");

    foregroundMaskAnalysisLabel.setForeground(new java.awt.Color(0, 0, 255));
    foregroundMaskAnalysisLabel.setText("C");
    foregroundMaskAnalysisLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    foregroundMaskAnalysisLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        foregroundMaskAnalysisLabelMouseClicked(evt);
      }
    });

    foregroundMaskAnalysisCheckBox.setText("Foreground Mask Analysis");
    foregroundMaskAnalysisCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        foregroundMaskAnalysisCheckBoxActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel12Layout = new javax.swing.GroupLayout(jPanel12);
    jPanel12.setLayout(jPanel12Layout);
    jPanel12Layout.setHorizontalGroup(
      jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel12Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel12Layout.createSequentialGroup()
            .addComponent(preProcessorLabel)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel12Layout.createSequentialGroup()
                .addGap(21, 21, 21)
                .addComponent(jLabel34, javax.swing.GroupLayout.PREFERRED_SIZE, 72, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(22, 22, 22)
                .addComponent(tictocComboBox, 0, 143, Short.MAX_VALUE))
              .addComponent(preProcessorCheckBox)))
          .addGroup(jPanel12Layout.createSequentialGroup()
            .addComponent(foregroundMaskAnalysisLabel)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(foregroundMaskAnalysisCheckBox)
            .addGap(0, 0, Short.MAX_VALUE)))
        .addContainerGap())
    );
    jPanel12Layout.setVerticalGroup(
      jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel12Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(preProcessorCheckBox)
          .addComponent(preProcessorLabel))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(foregroundMaskAnalysisCheckBox)
          .addComponent(foregroundMaskAnalysisLabel))
        .addGap(1, 1, 1)
        .addGroup(jPanel12Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(tictocComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel34))
        .addContainerGap(14, Short.MAX_VALUE))
    );

    jPanel13.setBorder(javax.swing.BorderFactory.createTitledBorder("Background Subtraction Methods"));

    frameDifferenceCheckBox.setText("Frame Difference");
    frameDifferenceCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        frameDifferenceCheckBoxActionPerformed(evt);
      }
    });

    staticFrameDifferenceCheckBox.setText("Static Frame Difference");
    staticFrameDifferenceCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        staticFrameDifferenceCheckBoxActionPerformed(evt);
      }
    });

    weightedMovingMeanCheckBox.setText("Weighted Moving Mean");
    weightedMovingMeanCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        weightedMovingMeanCheckBoxActionPerformed(evt);
      }
    });

    weightedMovingVarianceCheckBox.setText("Weighted Moving Variance");
    weightedMovingVarianceCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        weightedMovingVarianceCheckBoxActionPerformed(evt);
      }
    });

    openCVMoGV1CheckBox.setText("OpenCV MoG V1");
    openCVMoGV1CheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        openCVMoGV1CheckBoxActionPerformed(evt);
      }
    });

    openCVMoGV2CheckBox.setText("OpenCV MoG V2");
    openCVMoGV2CheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        openCVMoGV2CheckBoxActionPerformed(evt);
      }
    });

    adaptiveBackgroundLearningCheckBox.setText("Adaptive Background Learning");
    adaptiveBackgroundLearningCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        adaptiveBackgroundLearningCheckBoxActionPerformed(evt);
      }
    });

    gaussianAverageCheckBox.setText("Gaussian Average");
    gaussianAverageCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        gaussianAverageCheckBoxActionPerformed(evt);
      }
    });

    eigenbackgroundCheckBox.setText("SL-PCA / Eigenbackground");
    eigenbackgroundCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        eigenbackgroundCheckBoxActionPerformed(evt);
      }
    });

    temporalMedianCheckBox.setText("Temporal Median");
    temporalMedianCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        temporalMedianCheckBoxActionPerformed(evt);
      }
    });

    adaptiveMedianCheckBox.setText("Adaptive Median");
    adaptiveMedianCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        adaptiveMedianCheckBoxActionPerformed(evt);
      }
    });

    temporalMeanCheckBox.setText("Temporal Mean");
    temporalMeanCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        temporalMeanCheckBoxActionPerformed(evt);
      }
    });

    grimsonGMMCheckBox.setText("Grimson GMM");
    grimsonGMMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        grimsonGMMCheckBoxActionPerformed(evt);
      }
    });

    zivkovicGMMCheckBox.setText("Zivkovic GMM");
    zivkovicGMMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        zivkovicGMMCheckBoxActionPerformed(evt);
      }
    });

    adaptiveSOMCheckBox.setText("Adaptive SOM");
    adaptiveSOMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        adaptiveSOMCheckBoxActionPerformed(evt);
      }
    });

    mixtureOfGaussiansCheckBox.setText("Mixture of Gaussians");
    mixtureOfGaussiansCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        mixtureOfGaussiansCheckBoxActionPerformed(evt);
      }
    });

    fuzzyGaussianCheckBox.setText("Fuzzy Gaussian");
    fuzzyGaussianCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        fuzzyGaussianCheckBoxActionPerformed(evt);
      }
    });

    simpleGaussianCheckBox.setText("Simple Gaussian");
    simpleGaussianCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        simpleGaussianCheckBoxActionPerformed(evt);
      }
    });

    fuzzyAdaptiveSOMCheckBox.setText("Fuzzy Adaptive SOM");
    fuzzyAdaptiveSOMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        fuzzyAdaptiveSOMCheckBoxActionPerformed(evt);
      }
    });

    T2FGMMUMCheckBox.setText("T2F GMM - UM");
    T2FGMMUMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        T2FGMMUMCheckBoxActionPerformed(evt);
      }
    });

    T2FGMMUVCheckBox.setText("T2F GMM - UV");
    T2FGMMUVCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        T2FGMMUVCheckBoxActionPerformed(evt);
      }
    });

    frameDifferenceLabel.setForeground(new java.awt.Color(0, 0, 255));
    frameDifferenceLabel.setText("C");
    frameDifferenceLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    frameDifferenceLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        frameDifferenceLabelMouseClicked(evt);
      }
    });

    staticFrameDifferenceLabel.setForeground(new java.awt.Color(0, 0, 255));
    staticFrameDifferenceLabel.setText("C");
    staticFrameDifferenceLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    staticFrameDifferenceLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        staticFrameDifferenceLabelMouseClicked(evt);
      }
    });

    weightedMovingMeanLabel.setForeground(new java.awt.Color(0, 0, 255));
    weightedMovingMeanLabel.setText("C");
    weightedMovingMeanLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    weightedMovingMeanLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        weightedMovingMeanLabelMouseClicked(evt);
      }
    });

    weightedMovingVarianceLabel.setForeground(new java.awt.Color(0, 0, 255));
    weightedMovingVarianceLabel.setText("C");
    weightedMovingVarianceLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    weightedMovingVarianceLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        weightedMovingVarianceLabelMouseClicked(evt);
      }
    });

    adaptiveBackgroundLearningLabel.setForeground(new java.awt.Color(0, 0, 255));
    adaptiveBackgroundLearningLabel.setText("C");
    adaptiveBackgroundLearningLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    adaptiveBackgroundLearningLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        adaptiveBackgroundLearningLabelMouseClicked(evt);
      }
    });

    openCVMoGV1Label.setForeground(new java.awt.Color(0, 0, 255));
    openCVMoGV1Label.setText("C");
    openCVMoGV1Label.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    openCVMoGV1Label.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        openCVMoGV1LabelMouseClicked(evt);
      }
    });

    openCVMoGV2Label.setForeground(new java.awt.Color(0, 0, 255));
    openCVMoGV2Label.setText("C");
    openCVMoGV2Label.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    openCVMoGV2Label.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        openCVMoGV2LabelMouseClicked(evt);
      }
    });

    zivkovicGMMLabel.setForeground(new java.awt.Color(0, 0, 255));
    zivkovicGMMLabel.setText("C");
    zivkovicGMMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    zivkovicGMMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        zivkovicGMMLabelMouseClicked(evt);
      }
    });

    grimsonGMMLabel.setForeground(new java.awt.Color(0, 0, 255));
    grimsonGMMLabel.setText("C");
    grimsonGMMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    grimsonGMMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        grimsonGMMLabelMouseClicked(evt);
      }
    });

    gaussianAverageLabel.setForeground(new java.awt.Color(0, 0, 255));
    gaussianAverageLabel.setText("C");
    gaussianAverageLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    gaussianAverageLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        gaussianAverageLabelMouseClicked(evt);
      }
    });

    eigenbackgroundLabel.setForeground(new java.awt.Color(0, 0, 255));
    eigenbackgroundLabel.setText("C");
    eigenbackgroundLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    eigenbackgroundLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        eigenbackgroundLabelMouseClicked(evt);
      }
    });

    temporalMedianLabel.setForeground(new java.awt.Color(0, 0, 255));
    temporalMedianLabel.setText("C");
    temporalMedianLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    temporalMedianLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        temporalMedianLabelMouseClicked(evt);
      }
    });

    adaptiveMedianLabel.setForeground(new java.awt.Color(0, 0, 255));
    adaptiveMedianLabel.setText("C");
    adaptiveMedianLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    adaptiveMedianLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        adaptiveMedianLabelMouseClicked(evt);
      }
    });

    temporalMeanLabel.setForeground(new java.awt.Color(0, 0, 255));
    temporalMeanLabel.setText("C");
    temporalMeanLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    temporalMeanLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        temporalMeanLabelMouseClicked(evt);
      }
    });

    simpleGaussianLabel.setForeground(new java.awt.Color(0, 0, 255));
    simpleGaussianLabel.setText("C");
    simpleGaussianLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    simpleGaussianLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        simpleGaussianLabelMouseClicked(evt);
      }
    });

    fuzzyGaussianLabel.setForeground(new java.awt.Color(0, 0, 255));
    fuzzyGaussianLabel.setText("C");
    fuzzyGaussianLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    fuzzyGaussianLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        fuzzyGaussianLabelMouseClicked(evt);
      }
    });

    mixtureOfGaussiansLabel.setForeground(new java.awt.Color(0, 0, 255));
    mixtureOfGaussiansLabel.setText("C");
    mixtureOfGaussiansLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    mixtureOfGaussiansLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        mixtureOfGaussiansLabelMouseClicked(evt);
      }
    });

    adaptiveSOMLabel.setForeground(new java.awt.Color(0, 0, 255));
    adaptiveSOMLabel.setText("C");
    adaptiveSOMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    adaptiveSOMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        adaptiveSOMLabelMouseClicked(evt);
      }
    });

    fuzzyAdaptiveSOMLabel.setForeground(new java.awt.Color(0, 0, 255));
    fuzzyAdaptiveSOMLabel.setText("C");
    fuzzyAdaptiveSOMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    fuzzyAdaptiveSOMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        fuzzyAdaptiveSOMLabelMouseClicked(evt);
      }
    });

    T2FGMMUMLabel.setForeground(new java.awt.Color(0, 0, 255));
    T2FGMMUMLabel.setText("C");
    T2FGMMUMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    T2FGMMUMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        T2FGMMUMLabelMouseClicked(evt);
      }
    });

    T2FGMMUVLabel.setForeground(new java.awt.Color(0, 0, 255));
    T2FGMMUVLabel.setText("C");
    T2FGMMUVLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    T2FGMMUVLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        T2FGMMUVLabelMouseClicked(evt);
      }
    });

    multiLayerBGSLabel.setForeground(new java.awt.Color(0, 0, 255));
    multiLayerBGSLabel.setText("C");
    multiLayerBGSLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    multiLayerBGSLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        multiLayerBGSLabelMouseClicked(evt);
      }
    });

    multiLayerBGSCheckBox.setText("Multi Layer BGS");
    multiLayerBGSCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        multiLayerBGSCheckBoxActionPerformed(evt);
      }
    });

    checkAllButton.setText("Check All");
    checkAllButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        checkAllButtonActionPerformed(evt);
      }
    });

    uncheckAllButton.setText("Uncheck All");
    uncheckAllButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        uncheckAllButtonActionPerformed(evt);
      }
    });

    openConfigButton.setText("Open");
    openConfigButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        openConfigButtonActionPerformed(evt);
      }
    });

    fuzzySugenoIntegralLabel.setForeground(new java.awt.Color(0, 0, 255));
    fuzzySugenoIntegralLabel.setText("C");
    fuzzySugenoIntegralLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    fuzzySugenoIntegralLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        fuzzySugenoIntegralLabelMouseClicked(evt);
      }
    });

    fuzzySugenoIntegralCheckBox.setText("Fuzzy Sugeno Integral");
    fuzzySugenoIntegralCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        fuzzySugenoIntegralCheckBoxActionPerformed(evt);
      }
    });

    fuzzyChoquetIntegralLabel.setForeground(new java.awt.Color(0, 0, 255));
    fuzzyChoquetIntegralLabel.setText("C");
    fuzzyChoquetIntegralLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    fuzzyChoquetIntegralLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        fuzzyChoquetIntegralLabelMouseClicked(evt);
      }
    });

    fuzzyChoquetIntegralCheckBox.setText("Fuzzy Choquet Integral");
    fuzzyChoquetIntegralCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        fuzzyChoquetIntegralCheckBoxActionPerformed(evt);
      }
    });

    T2FMRFUMLabel.setForeground(new java.awt.Color(0, 0, 255));
    T2FMRFUMLabel.setText("C");
    T2FMRFUMLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    T2FMRFUMLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        T2FMRFUMLabelMouseClicked(evt);
      }
    });

    T2FMRFUMCheckBox.setText("T2F MRF - UM");
    T2FMRFUMCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        T2FMRFUMCheckBoxActionPerformed(evt);
      }
    });

    T2FMRFUVLabel.setForeground(new java.awt.Color(0, 0, 255));
    T2FMRFUVLabel.setText("C");
    T2FMRFUVLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    T2FMRFUVLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        T2FMRFUVLabelMouseClicked(evt);
      }
    });

    T2FMRFUVCheckBox.setText("T2F MRF - UV");
    T2FMRFUVCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        T2FMRFUVCheckBoxActionPerformed(evt);
      }
    });

    GMGCheckBox.setText("GMG");
    GMGCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        GMGCheckBoxActionPerformed(evt);
      }
    });

    GMGLabel.setForeground(new java.awt.Color(0, 0, 255));
    GMGLabel.setText("C");
    GMGLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    GMGLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        GMGLabelMouseClicked(evt);
      }
    });

    VuMeterLabel.setForeground(new java.awt.Color(0, 0, 255));
    VuMeterLabel.setText("C");
    VuMeterLabel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
    VuMeterLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        VuMeterLabelMouseClicked(evt);
      }
    });

    VuMeterCheckBox.setText("VuMeter");
    VuMeterCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        VuMeterCheckBoxActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel13Layout = new javax.swing.GroupLayout(jPanel13);
    jPanel13.setLayout(jPanel13Layout);
    jPanel13Layout.setHorizontalGroup(
      jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel13Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel13Layout.createSequentialGroup()
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(frameDifferenceLabel, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(staticFrameDifferenceLabel, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(weightedMovingMeanLabel, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(weightedMovingVarianceLabel, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(adaptiveBackgroundLearningLabel, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(openCVMoGV1Label, javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(openCVMoGV2Label, javax.swing.GroupLayout.Alignment.TRAILING))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(staticFrameDifferenceCheckBox)
              .addComponent(weightedMovingMeanCheckBox)
              .addComponent(weightedMovingVarianceCheckBox)
              .addComponent(adaptiveBackgroundLearningCheckBox)
              .addComponent(frameDifferenceCheckBox)
              .addComponent(openCVMoGV1CheckBox)
              .addComponent(openCVMoGV2CheckBox))
            .addGap(18, 18, 18)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(grimsonGMMLabel)
              .addComponent(zivkovicGMMLabel)
              .addComponent(temporalMeanLabel)
              .addComponent(gaussianAverageLabel)
              .addComponent(eigenbackgroundLabel)
              .addComponent(temporalMedianLabel)
              .addComponent(adaptiveMedianLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(grimsonGMMCheckBox)
              .addComponent(gaussianAverageCheckBox)
              .addComponent(eigenbackgroundCheckBox)
              .addComponent(temporalMedianCheckBox)
              .addComponent(zivkovicGMMCheckBox)
              .addComponent(adaptiveMedianCheckBox)
              .addComponent(temporalMeanCheckBox))
            .addGap(18, 18, 18)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(fuzzyGaussianLabel)
              .addComponent(simpleGaussianLabel)
              .addComponent(T2FGMMUVLabel)
              .addComponent(mixtureOfGaussiansLabel)
              .addComponent(adaptiveSOMLabel)
              .addComponent(fuzzyAdaptiveSOMLabel)
              .addComponent(T2FGMMUMLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(T2FGMMUVCheckBox)
              .addComponent(T2FGMMUMCheckBox)
              .addComponent(fuzzyAdaptiveSOMCheckBox)
              .addComponent(simpleGaussianCheckBox)
              .addComponent(mixtureOfGaussiansCheckBox)
              .addComponent(fuzzyGaussianCheckBox)
              .addComponent(adaptiveSOMCheckBox))
            .addGap(18, 18, 18)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addComponent(fuzzySugenoIntegralLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(fuzzySugenoIntegralCheckBox))
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addComponent(fuzzyChoquetIntegralLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(fuzzyChoquetIntegralCheckBox))
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addComponent(multiLayerBGSLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(multiLayerBGSCheckBox))
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addComponent(T2FMRFUVLabel)
                  .addComponent(T2FMRFUMLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addComponent(T2FMRFUVCheckBox)
                  .addComponent(T2FMRFUMCheckBox)))
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addComponent(GMGLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(GMGCheckBox))
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addComponent(VuMeterLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(VuMeterCheckBox)))
            .addGap(0, 0, Short.MAX_VALUE))
          .addGroup(jPanel13Layout.createSequentialGroup()
            .addComponent(checkAllButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(uncheckAllButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(openConfigButton, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addContainerGap())
    );
    jPanel13Layout.setVerticalGroup(
      jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel13Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel13Layout.createSequentialGroup()
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(frameDifferenceCheckBox)
              .addComponent(temporalMeanCheckBox)
              .addComponent(simpleGaussianCheckBox)
              .addComponent(frameDifferenceLabel)
              .addComponent(temporalMeanLabel)
              .addComponent(simpleGaussianLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(staticFrameDifferenceCheckBox)
              .addComponent(adaptiveMedianCheckBox)
              .addComponent(fuzzyGaussianCheckBox)
              .addComponent(staticFrameDifferenceLabel)
              .addComponent(adaptiveMedianLabel)
              .addComponent(fuzzyGaussianLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(weightedMovingMeanCheckBox)
              .addComponent(temporalMedianCheckBox)
              .addComponent(mixtureOfGaussiansCheckBox)
              .addComponent(weightedMovingMeanLabel)
              .addComponent(temporalMedianLabel)
              .addComponent(mixtureOfGaussiansLabel))
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(weightedMovingVarianceCheckBox)
              .addComponent(eigenbackgroundCheckBox)
              .addComponent(adaptiveSOMCheckBox)
              .addComponent(weightedMovingVarianceLabel)
              .addComponent(eigenbackgroundLabel)
              .addComponent(adaptiveSOMLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(adaptiveBackgroundLearningCheckBox)
              .addComponent(gaussianAverageCheckBox)
              .addComponent(fuzzyAdaptiveSOMCheckBox)
              .addComponent(adaptiveBackgroundLearningLabel)
              .addComponent(gaussianAverageLabel)
              .addComponent(fuzzyAdaptiveSOMLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(openCVMoGV1CheckBox)
              .addComponent(grimsonGMMCheckBox)
              .addComponent(T2FGMMUMCheckBox)
              .addComponent(openCVMoGV1Label)
              .addComponent(grimsonGMMLabel)
              .addComponent(T2FGMMUMLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(openCVMoGV2CheckBox)
              .addComponent(zivkovicGMMCheckBox)
              .addComponent(T2FGMMUVCheckBox)
              .addComponent(openCVMoGV2Label)
              .addComponent(zivkovicGMMLabel)
              .addComponent(T2FGMMUVLabel)))
          .addGroup(jPanel13Layout.createSequentialGroup()
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(T2FMRFUMCheckBox)
              .addComponent(T2FMRFUMLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(T2FMRFUVCheckBox)
              .addComponent(T2FMRFUVLabel))
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel13Layout.createSequentialGroup()
                .addGap(24, 24, 24)
                .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                  .addComponent(fuzzyChoquetIntegralCheckBox)
                  .addComponent(fuzzyChoquetIntegralLabel))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                  .addComponent(multiLayerBGSCheckBox)
                  .addComponent(multiLayerBGSLabel)))
              .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                .addComponent(fuzzySugenoIntegralCheckBox)
                .addComponent(fuzzySugenoIntegralLabel)))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(GMGCheckBox)
              .addComponent(GMGLabel))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(VuMeterCheckBox)
              .addComponent(VuMeterLabel))))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 30, Short.MAX_VALUE)
        .addGroup(jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(checkAllButton)
          .addComponent(uncheckAllButton)
          .addComponent(openConfigButton))
        .addContainerGap())
    );

    javax.swing.GroupLayout jPanel11Layout = new javax.swing.GroupLayout(jPanel11);
    jPanel11.setLayout(jPanel11Layout);
    jPanel11Layout.setHorizontalGroup(
      jPanel11Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel11Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel11Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(jLabel11, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
          .addGroup(jPanel11Layout.createSequentialGroup()
            .addGroup(jPanel11Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(jPanel13, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addComponent(jPanel12, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGap(0, 0, Short.MAX_VALUE)))
        .addContainerGap())
    );
    jPanel11Layout.setVerticalGroup(
      jPanel11Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel11Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jLabel11)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jPanel12, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jPanel13, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        .addContainerGap())
    );

    runButton2.setText("Run");
    runButton2.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        runButton2ActionPerformed(evt);
      }
    });

    javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
    jPanel2.setLayout(jPanel2Layout);
    jPanel2Layout.setHorizontalGroup(
      jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel2Layout.createSequentialGroup()
            .addComponent(jPanel11, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(0, 0, Short.MAX_VALUE))
          .addGroup(jPanel2Layout.createSequentialGroup()
            .addComponent(jPanel9, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jPanel10, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(reloadButton, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, 80, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addComponent(runButton2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.PREFERRED_SIZE, 80, javax.swing.GroupLayout.PREFERRED_SIZE))))
        .addContainerGap())
    );
    jPanel2Layout.setVerticalGroup(
      jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel2Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel2Layout.createSequentialGroup()
            .addComponent(reloadButton)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(runButton2))
          .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
            .addComponent(jPanel10, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jPanel9, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jPanel11, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        .addContainerGap())
    );

    appTabbedPane.addTab("Config", jPanel2);

    jToolBar1.setFloatable(false);

    saveButton.setText("Save");
    saveButton.setFocusable(false);
    saveButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    saveButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    saveButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        saveButtonActionPerformed(evt);
      }
    });
    jToolBar1.add(saveButton);
    jToolBar1.add(jSeparator1);

    returnButton.setText("Return");
    returnButton.setFocusable(false);
    returnButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    returnButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    returnButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        returnButtonActionPerformed(evt);
      }
    });
    jToolBar1.add(returnButton);
    jToolBar1.add(jSeparator3);

    runButton3.setText("Run");
    runButton3.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        runButton3ActionPerformed(evt);
      }
    });
    jToolBar1.add(runButton3);

    jPanel17.setBorder(javax.swing.BorderFactory.createTitledBorder("File Content"));

    jScrollPane1.setAutoscrolls(true);
    jScrollPane1.setDoubleBuffered(true);

    fileContentTextPane.setDoubleBuffered(true);
    jScrollPane1.setViewportView(fileContentTextPane);

    jLabel12.setText("File name:");

    fileNameLabel.setText("_");

    javax.swing.GroupLayout jPanel17Layout = new javax.swing.GroupLayout(jPanel17);
    jPanel17.setLayout(jPanel17Layout);
    jPanel17Layout.setHorizontalGroup(
      jPanel17Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel17Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel17Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel17Layout.createSequentialGroup()
            .addComponent(jLabel12)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(fileNameLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 200, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 710, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(18, Short.MAX_VALUE))
    );
    jPanel17Layout.setVerticalGroup(
      jPanel17Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel17Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 487, Short.MAX_VALUE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel17Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel12)
          .addComponent(fileNameLabel))
        .addContainerGap())
    );

    javax.swing.GroupLayout jPanel16Layout = new javax.swing.GroupLayout(jPanel16);
    jPanel16.setLayout(jPanel16Layout);
    jPanel16Layout.setHorizontalGroup(
      jPanel16Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel16Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel16Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel16Layout.createSequentialGroup()
            .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(0, 0, Short.MAX_VALUE))
          .addComponent(jPanel17, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        .addContainerGap())
    );
    jPanel16Layout.setVerticalGroup(
      jPanel16Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel16Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jPanel17, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        .addContainerGap())
    );

    appTabbedPane.addTab("Editor", jPanel16);

    logTextScrollPane.setDoubleBuffered(true);

    logTextPane.setEditable(false);
    logTextPane.setDoubleBuffered(true);
    logTextScrollPane.setViewportView(logTextPane);

    jToolBar2.setFloatable(false);

    clearButton.setText("Clear");
    clearButton.setFocusable(false);
    clearButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    clearButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    clearButton.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        clearButtonActionPerformed(evt);
      }
    });
    jToolBar2.add(clearButton);
    jToolBar2.add(jSeparator4);

    runButton4.setText("Run");
    runButton4.setFocusable(false);
    runButton4.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    runButton4.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    runButton4.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        runButton4ActionPerformed(evt);
      }
    });
    jToolBar2.add(runButton4);
    jToolBar2.add(jSeparator2);

    returnButton2.setText("Return");
    returnButton2.setFocusable(false);
    returnButton2.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    returnButton2.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    returnButton2.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        returnButton2ActionPerformed(evt);
      }
    });
    jToolBar2.add(returnButton2);
    jToolBar2.add(jSeparator5);

    autoscrollCheckBox.setFocusable(false);
    autoscrollCheckBox.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    autoscrollCheckBox.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    autoscrollCheckBox.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        autoscrollCheckBoxActionPerformed(evt);
      }
    });
    jToolBar2.add(autoscrollCheckBox);

    autoscrollLabel.setText(" Autoscroll");
    autoscrollLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        autoscrollLabelMouseClicked(evt);
      }
    });
    jToolBar2.add(autoscrollLabel);
    jToolBar2.add(jSeparator6);

    autoclearCheckBox.setFocusable(false);
    autoclearCheckBox.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
    autoclearCheckBox.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
    jToolBar2.add(autoclearCheckBox);

    autoclearLabel.setText(" Autoclear");
    autoclearLabel.addMouseListener(new java.awt.event.MouseAdapter()
    {
      public void mouseClicked(java.awt.event.MouseEvent evt)
      {
        autoclearLabelMouseClicked(evt);
      }
    });
    jToolBar2.add(autoclearLabel);

    javax.swing.GroupLayout jPanel18Layout = new javax.swing.GroupLayout(jPanel18);
    jPanel18.setLayout(jPanel18Layout);
    jPanel18Layout.setHorizontalGroup(
      jPanel18Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel18Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel18Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(logTextScrollPane, javax.swing.GroupLayout.PREFERRED_SIZE, 741, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jToolBar2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(19, Short.MAX_VALUE))
    );
    jPanel18Layout.setVerticalGroup(
      jPanel18Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel18Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jToolBar2, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(logTextScrollPane, javax.swing.GroupLayout.DEFAULT_SIZE, 552, Short.MAX_VALUE)
        .addContainerGap())
    );

    appTabbedPane.addTab("Log", jPanel18);

    jMenu3.setText("App");

    jMenuItem1.setText("Exit");
    jMenuItem1.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuItem1ActionPerformed(evt);
      }
    });
    jMenu3.add(jMenuItem1);

    jMenuBar2.add(jMenu3);

    jMenu4.setText("Help");

    jMenuItem2.setText("About");
    jMenuItem2.addActionListener(new java.awt.event.ActionListener()
    {
      public void actionPerformed(java.awt.event.ActionEvent evt)
      {
        jMenuItem2ActionPerformed(evt);
      }
    });
    jMenu4.add(jMenuItem2);

    jMenuBar2.add(jMenu4);

    setJMenuBar(jMenuBar2);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(appTabbedPane, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(appTabbedPane)
        .addContainerGap())
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

  private AboutDialog dialog = null;
  
  private void jMenuItem2ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItem2ActionPerformed
  {//GEN-HEADEREND:event_jMenuItem2ActionPerformed
    java.awt.EventQueue.invokeLater(new Runnable()
    {
      @Override
      public void run()
      {
        dialog = new AboutDialog(new javax.swing.JFrame(), true);
        dialog.addWindowListener(new java.awt.event.WindowAdapter()
        {
          @Override
          public void windowClosing(java.awt.event.WindowEvent e)
          {
            //System.exit(0);
          }
        });
        dialog.setVisible(true);
      }
    });
  }//GEN-LAST:event_jMenuItem2ActionPerformed

  private void jMenuItem1ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_jMenuItem1ActionPerformed
  {//GEN-HEADEREND:event_jMenuItem1ActionPerformed
    System.exit(0);
  }//GEN-LAST:event_jMenuItem1ActionPerformed

  private void returnButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_returnButtonActionPerformed
  {//GEN-HEADEREND:event_returnButtonActionPerformed
    retur(true);
  }//GEN-LAST:event_returnButtonActionPerformed

  private void useCameraRadioButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_useCameraRadioButtonActionPerformed
  {//GEN-HEADEREND:event_useCameraRadioButtonActionPerformed
    useCameraRadioButton.setSelected(true);
    useVideoRadioButton.setSelected(false);
    useDatasetRadioButton.setSelected(false);
    Command.setUseCamera();
    update();
  }//GEN-LAST:event_useCameraRadioButtonActionPerformed

  private void useVideoRadioButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_useVideoRadioButtonActionPerformed
  {//GEN-HEADEREND:event_useVideoRadioButtonActionPerformed
    useCameraRadioButton.setSelected(false);
    useVideoRadioButton.setSelected(true);
    useDatasetRadioButton.setSelected(false);
    Command.setUseVideo();
    update();
  }//GEN-LAST:event_useVideoRadioButtonActionPerformed

  private void useDatasetRadioButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_useDatasetRadioButtonActionPerformed
  {//GEN-HEADEREND:event_useDatasetRadioButtonActionPerformed
    useCameraRadioButton.setSelected(false);
    useVideoRadioButton.setSelected(false);
    useDatasetRadioButton.setSelected(true);
    Command.setUseDataset();
    update();
  }//GEN-LAST:event_useDatasetRadioButtonActionPerformed

  private void setInputFileButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_setInputFileButtonActionPerformed
  {//GEN-HEADEREND:event_setInputFileButtonActionPerformed
    useVideoRadioButtonActionPerformed(null);
    Command.setInputFile(getFilePath(false));
    update();
  }//GEN-LAST:event_setInputFileButtonActionPerformed

  private void setDatasetFolderButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_setDatasetFolderButtonActionPerformed
  {//GEN-HEADEREND:event_setDatasetFolderButtonActionPerformed
    useDatasetRadioButtonActionPerformed(null);
    Command.setDatasetFolder(getFilePath(true));
    update();
  }//GEN-LAST:event_setDatasetFolderButtonActionPerformed

  private void setTrainFileButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_setTrainFileButtonActionPerformed
  {//GEN-HEADEREND:event_setTrainFileButtonActionPerformed
    useDatasetRadioButtonActionPerformed(null);
    Command.setTrainFile(getFilePath(false));
    update();
  }//GEN-LAST:event_setTrainFileButtonActionPerformed

  private void setTestFileButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_setTestFileButtonActionPerformed
  {//GEN-HEADEREND:event_setTestFileButtonActionPerformed
    useDatasetRadioButtonActionPerformed(null);
    Command.setTestFile(getFilePath(false));
    update();
  }//GEN-LAST:event_setTestFileButtonActionPerformed

  private void useForegroundMaskAnalysisCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_useForegroundMaskAnalysisCheckBoxActionPerformed
  {//GEN-HEADEREND:event_useForegroundMaskAnalysisCheckBoxActionPerformed
    Command.setUseForegroundMaskAnalysis(useForegroundMaskAnalysisCheckBox.isSelected());
    Command.setCompareAt(Integer.parseInt(compareAtTextField.getText()));
    update();
  }//GEN-LAST:event_useForegroundMaskAnalysisCheckBoxActionPerformed

  private void setImageRefButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_setImageRefButtonActionPerformed
  {//GEN-HEADEREND:event_setImageRefButtonActionPerformed
    Command.setImageRefFile(getFilePath(false));
    Command.setCompareAt(Integer.parseInt(compareAtTextField.getText()));
    update();
  }//GEN-LAST:event_setImageRefButtonActionPerformed

  private void compareAtTextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_compareAtTextFieldFocusLost
  {//GEN-HEADEREND:event_compareAtTextFieldFocusLost
    Command.setCompareAt(Integer.parseInt(compareAtTextField.getText()));
    update();
  }//GEN-LAST:event_compareAtTextFieldFocusLost

  private void saveButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_saveButtonActionPerformed
  {//GEN-HEADEREND:event_saveButtonActionPerformed
    save();
  }//GEN-LAST:event_saveButtonActionPerformed

  private void reloadButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_reloadButtonActionPerformed
  {//GEN-HEADEREND:event_reloadButtonActionPerformed
    reload();
  }//GEN-LAST:event_reloadButtonActionPerformed

  private void preProcessorLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_preProcessorLabelMouseClicked
  {//GEN-HEADEREND:event_preProcessorLabelMouseClicked
    openFileContent(Configuration.PreProcessor);
  }//GEN-LAST:event_preProcessorLabelMouseClicked

  private void resizeFrameTextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_resizeFrameTextFieldFocusLost
  {//GEN-HEADEREND:event_resizeFrameTextFieldFocusLost
    changeParam("input_resize_percent", resizeFrameTextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_resizeFrameTextFieldFocusLost

  private void stopVideoAtTextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_stopVideoAtTextFieldFocusLost
  {//GEN-HEADEREND:event_stopVideoAtTextFieldFocusLost
    changeParam("stopAt", stopVideoAtTextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_stopVideoAtTextFieldFocusLost

  private void verticalFlipCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_verticalFlipCheckBoxActionPerformed
  {//GEN-HEADEREND:event_verticalFlipCheckBoxActionPerformed
    changeParam("enableFlip", verticalFlipCheckBox, Configuration.VideoCapture);
  }//GEN-LAST:event_verticalFlipCheckBoxActionPerformed

  private void showOutputCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_showOutputCheckBoxActionPerformed
  {//GEN-HEADEREND:event_showOutputCheckBoxActionPerformed
    changeParam("showOutput", showOutputCheckBox, Configuration.VideoCapture);
  }//GEN-LAST:event_showOutputCheckBoxActionPerformed

  private void useROICheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_useROICheckBoxActionPerformed
  {//GEN-HEADEREND:event_useROICheckBoxActionPerformed
    changeParam("use_roi", useROICheckBox, Configuration.VideoCapture);
  }//GEN-LAST:event_useROICheckBoxActionPerformed

  private void ROIdefinedCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_ROIdefinedCheckBoxActionPerformed
  {//GEN-HEADEREND:event_ROIdefinedCheckBoxActionPerformed
    changeParam("roi_defined", ROIdefinedCheckBox, Configuration.VideoCapture);
  }//GEN-LAST:event_ROIdefinedCheckBoxActionPerformed

  private void x0ROITextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_x0ROITextFieldFocusLost
  {//GEN-HEADEREND:event_x0ROITextFieldFocusLost
    changeParam("roi_x0", x0ROITextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_x0ROITextFieldFocusLost

  private void x1ROITextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_x1ROITextFieldFocusLost
  {//GEN-HEADEREND:event_x1ROITextFieldFocusLost
    changeParam("roi_x1", x1ROITextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_x1ROITextFieldFocusLost

  private void y0ROITextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_y0ROITextFieldFocusLost
  {//GEN-HEADEREND:event_y0ROITextFieldFocusLost
    changeParam("roi_y0", y0ROITextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_y0ROITextFieldFocusLost

  private void y1ROITextFieldFocusLost(java.awt.event.FocusEvent evt)//GEN-FIRST:event_y1ROITextFieldFocusLost
  {//GEN-HEADEREND:event_y1ROITextFieldFocusLost
    changeParam("roi_y1", y1ROITextField.getText(), Configuration.VideoCapture);
  }//GEN-LAST:event_y1ROITextFieldFocusLost

  private void preProcessorCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_preProcessorCheckBoxActionPerformed
  {//GEN-HEADEREND:event_preProcessorCheckBoxActionPerformed
    changeParam("enablePreProcessor", preProcessorCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_preProcessorCheckBoxActionPerformed

  private void tictocComboBoxItemStateChanged(java.awt.event.ItemEvent evt)//GEN-FIRST:event_tictocComboBoxItemStateChanged
  {//GEN-HEADEREND:event_tictocComboBoxItemStateChanged
    changeParam("tictoc", tictocComboBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_tictocComboBoxItemStateChanged

  private void frameDifferenceCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_frameDifferenceCheckBoxActionPerformed
  {//GEN-HEADEREND:event_frameDifferenceCheckBoxActionPerformed
    changeParam("enableFrameDifferenceBGS", frameDifferenceCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_frameDifferenceCheckBoxActionPerformed

  private void staticFrameDifferenceCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_staticFrameDifferenceCheckBoxActionPerformed
  {//GEN-HEADEREND:event_staticFrameDifferenceCheckBoxActionPerformed
    changeParam("enableStaticFrameDifferenceBGS", staticFrameDifferenceCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_staticFrameDifferenceCheckBoxActionPerformed

  private void weightedMovingMeanCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_weightedMovingMeanCheckBoxActionPerformed
  {//GEN-HEADEREND:event_weightedMovingMeanCheckBoxActionPerformed
    changeParam("enableWeightedMovingMeanBGS", weightedMovingMeanCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_weightedMovingMeanCheckBoxActionPerformed

  private void weightedMovingVarianceCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_weightedMovingVarianceCheckBoxActionPerformed
  {//GEN-HEADEREND:event_weightedMovingVarianceCheckBoxActionPerformed
    changeParam("enableWeightedMovingVarianceBGS", weightedMovingVarianceCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_weightedMovingVarianceCheckBoxActionPerformed

  private void adaptiveBackgroundLearningCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_adaptiveBackgroundLearningCheckBoxActionPerformed
  {//GEN-HEADEREND:event_adaptiveBackgroundLearningCheckBoxActionPerformed
    changeParam("enableAdaptiveBackgroundLearning", adaptiveBackgroundLearningCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_adaptiveBackgroundLearningCheckBoxActionPerformed

  private void openCVMoGV1CheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_openCVMoGV1CheckBoxActionPerformed
  {//GEN-HEADEREND:event_openCVMoGV1CheckBoxActionPerformed
    changeParam("enableMixtureOfGaussianV1BGS", openCVMoGV1CheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_openCVMoGV1CheckBoxActionPerformed

  private void openCVMoGV2CheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_openCVMoGV2CheckBoxActionPerformed
  {//GEN-HEADEREND:event_openCVMoGV2CheckBoxActionPerformed
    changeParam("enableMixtureOfGaussianV2BGS", openCVMoGV2CheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_openCVMoGV2CheckBoxActionPerformed

  private void temporalMeanCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_temporalMeanCheckBoxActionPerformed
  {//GEN-HEADEREND:event_temporalMeanCheckBoxActionPerformed
    changeParam("enableDPMeanBGS", temporalMeanCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_temporalMeanCheckBoxActionPerformed

  private void adaptiveMedianCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_adaptiveMedianCheckBoxActionPerformed
  {//GEN-HEADEREND:event_adaptiveMedianCheckBoxActionPerformed
    changeParam("enableDPAdaptiveMedianBGS", adaptiveMedianCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_adaptiveMedianCheckBoxActionPerformed

  private void temporalMedianCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_temporalMedianCheckBoxActionPerformed
  {//GEN-HEADEREND:event_temporalMedianCheckBoxActionPerformed
    changeParam("enableDPPratiMediodBGS", temporalMedianCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_temporalMedianCheckBoxActionPerformed

  private void eigenbackgroundCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_eigenbackgroundCheckBoxActionPerformed
  {//GEN-HEADEREND:event_eigenbackgroundCheckBoxActionPerformed
    changeParam("enableDPEigenbackgroundBGS", eigenbackgroundCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_eigenbackgroundCheckBoxActionPerformed

  private void gaussianAverageCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_gaussianAverageCheckBoxActionPerformed
  {//GEN-HEADEREND:event_gaussianAverageCheckBoxActionPerformed
    changeParam("enableDPWrenGABGS", gaussianAverageCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_gaussianAverageCheckBoxActionPerformed

  private void grimsonGMMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_grimsonGMMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_grimsonGMMCheckBoxActionPerformed
    changeParam("enableDPGrimsonGMMBGS", grimsonGMMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_grimsonGMMCheckBoxActionPerformed

  private void zivkovicGMMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_zivkovicGMMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_zivkovicGMMCheckBoxActionPerformed
    changeParam("enableDPZivkovicAGMMBGS", zivkovicGMMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_zivkovicGMMCheckBoxActionPerformed

  private void simpleGaussianCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_simpleGaussianCheckBoxActionPerformed
  {//GEN-HEADEREND:event_simpleGaussianCheckBoxActionPerformed
    changeParam("enableLBSimpleGaussian", simpleGaussianCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_simpleGaussianCheckBoxActionPerformed

  private void fuzzyGaussianCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_fuzzyGaussianCheckBoxActionPerformed
  {//GEN-HEADEREND:event_fuzzyGaussianCheckBoxActionPerformed
    changeParam("enableLBFuzzyGaussian", fuzzyGaussianCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_fuzzyGaussianCheckBoxActionPerformed

  private void mixtureOfGaussiansCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_mixtureOfGaussiansCheckBoxActionPerformed
  {//GEN-HEADEREND:event_mixtureOfGaussiansCheckBoxActionPerformed
    changeParam("enableLBMixtureOfGaussians", mixtureOfGaussiansCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_mixtureOfGaussiansCheckBoxActionPerformed

  private void adaptiveSOMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_adaptiveSOMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_adaptiveSOMCheckBoxActionPerformed
    changeParam("enableLBAdaptiveSOM", adaptiveSOMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_adaptiveSOMCheckBoxActionPerformed

  private void fuzzyAdaptiveSOMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_fuzzyAdaptiveSOMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_fuzzyAdaptiveSOMCheckBoxActionPerformed
    changeParam("enableLBFuzzyAdaptiveSOM", fuzzyAdaptiveSOMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_fuzzyAdaptiveSOMCheckBoxActionPerformed

  private void T2FGMMUMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_T2FGMMUMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_T2FGMMUMCheckBoxActionPerformed
    changeParam("enableT2FGMM_UM", T2FGMMUMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_T2FGMMUMCheckBoxActionPerformed

  private void T2FGMMUVCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_T2FGMMUVCheckBoxActionPerformed
  {//GEN-HEADEREND:event_T2FGMMUVCheckBoxActionPerformed
    changeParam("enableT2FGMM_UV", T2FGMMUVCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_T2FGMMUVCheckBoxActionPerformed

  private void multiLayerBGSCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_multiLayerBGSCheckBoxActionPerformed
  {//GEN-HEADEREND:event_multiLayerBGSCheckBoxActionPerformed
    changeParam("enableMultiLayerBGS", multiLayerBGSCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_multiLayerBGSCheckBoxActionPerformed

  private void foregroundMaskAnalysisCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_foregroundMaskAnalysisCheckBoxActionPerformed
  {//GEN-HEADEREND:event_foregroundMaskAnalysisCheckBoxActionPerformed
    changeParam("enableForegroundMaskAnalysis", foregroundMaskAnalysisCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_foregroundMaskAnalysisCheckBoxActionPerformed

  private void frameDifferenceLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_frameDifferenceLabelMouseClicked
  {//GEN-HEADEREND:event_frameDifferenceLabelMouseClicked
    openFileContent(Configuration.FrameDifferenceBGS);
  }//GEN-LAST:event_frameDifferenceLabelMouseClicked

  private void staticFrameDifferenceLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_staticFrameDifferenceLabelMouseClicked
  {//GEN-HEADEREND:event_staticFrameDifferenceLabelMouseClicked
    openFileContent(Configuration.StaticFrameDifferenceBGS);
  }//GEN-LAST:event_staticFrameDifferenceLabelMouseClicked

  private void weightedMovingMeanLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_weightedMovingMeanLabelMouseClicked
  {//GEN-HEADEREND:event_weightedMovingMeanLabelMouseClicked
    openFileContent(Configuration.WeightedMovingMeanBGS);
  }//GEN-LAST:event_weightedMovingMeanLabelMouseClicked

  private void weightedMovingVarianceLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_weightedMovingVarianceLabelMouseClicked
  {//GEN-HEADEREND:event_weightedMovingVarianceLabelMouseClicked
    openFileContent(Configuration.WeightedMovingVarianceBGS);
  }//GEN-LAST:event_weightedMovingVarianceLabelMouseClicked

  private void adaptiveBackgroundLearningLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_adaptiveBackgroundLearningLabelMouseClicked
  {//GEN-HEADEREND:event_adaptiveBackgroundLearningLabelMouseClicked
    openFileContent(Configuration.AdaptiveBackgroundLearning);
  }//GEN-LAST:event_adaptiveBackgroundLearningLabelMouseClicked

  private void openCVMoGV1LabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_openCVMoGV1LabelMouseClicked
  {//GEN-HEADEREND:event_openCVMoGV1LabelMouseClicked
    openFileContent(Configuration.MixtureOfGaussianV1BGS);
  }//GEN-LAST:event_openCVMoGV1LabelMouseClicked

  private void openCVMoGV2LabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_openCVMoGV2LabelMouseClicked
  {//GEN-HEADEREND:event_openCVMoGV2LabelMouseClicked
    openFileContent(Configuration.MixtureOfGaussianV2BGS);
  }//GEN-LAST:event_openCVMoGV2LabelMouseClicked

  private void temporalMeanLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_temporalMeanLabelMouseClicked
  {//GEN-HEADEREND:event_temporalMeanLabelMouseClicked
    openFileContent(Configuration.DPMeanBGS);
  }//GEN-LAST:event_temporalMeanLabelMouseClicked

  private void adaptiveMedianLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_adaptiveMedianLabelMouseClicked
  {//GEN-HEADEREND:event_adaptiveMedianLabelMouseClicked
    openFileContent(Configuration.DPAdaptiveMedianBGS);
  }//GEN-LAST:event_adaptiveMedianLabelMouseClicked

  private void temporalMedianLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_temporalMedianLabelMouseClicked
  {//GEN-HEADEREND:event_temporalMedianLabelMouseClicked
    openFileContent(Configuration.DPPratiMediodBGS);
  }//GEN-LAST:event_temporalMedianLabelMouseClicked

  private void eigenbackgroundLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_eigenbackgroundLabelMouseClicked
  {//GEN-HEADEREND:event_eigenbackgroundLabelMouseClicked
    openFileContent(Configuration.DPEigenbackgroundBGS);
  }//GEN-LAST:event_eigenbackgroundLabelMouseClicked

  private void gaussianAverageLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_gaussianAverageLabelMouseClicked
  {//GEN-HEADEREND:event_gaussianAverageLabelMouseClicked
    openFileContent(Configuration.DPWrenGABGS);
  }//GEN-LAST:event_gaussianAverageLabelMouseClicked

  private void grimsonGMMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_grimsonGMMLabelMouseClicked
  {//GEN-HEADEREND:event_grimsonGMMLabelMouseClicked
    openFileContent(Configuration.DPGrimsonGMMBGS);
  }//GEN-LAST:event_grimsonGMMLabelMouseClicked

  private void zivkovicGMMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_zivkovicGMMLabelMouseClicked
  {//GEN-HEADEREND:event_zivkovicGMMLabelMouseClicked
    openFileContent(Configuration.DPZivkovicAGMMBGS);
  }//GEN-LAST:event_zivkovicGMMLabelMouseClicked

  private void simpleGaussianLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_simpleGaussianLabelMouseClicked
  {//GEN-HEADEREND:event_simpleGaussianLabelMouseClicked
    openFileContent(Configuration.LBSimpleGaussian);
  }//GEN-LAST:event_simpleGaussianLabelMouseClicked

  private void fuzzyGaussianLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_fuzzyGaussianLabelMouseClicked
  {//GEN-HEADEREND:event_fuzzyGaussianLabelMouseClicked
    openFileContent(Configuration.LBFuzzyGaussian);
  }//GEN-LAST:event_fuzzyGaussianLabelMouseClicked

  private void mixtureOfGaussiansLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_mixtureOfGaussiansLabelMouseClicked
  {//GEN-HEADEREND:event_mixtureOfGaussiansLabelMouseClicked
    openFileContent(Configuration.LBMixtureOfGaussians);
  }//GEN-LAST:event_mixtureOfGaussiansLabelMouseClicked

  private void adaptiveSOMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_adaptiveSOMLabelMouseClicked
  {//GEN-HEADEREND:event_adaptiveSOMLabelMouseClicked
    openFileContent(Configuration.LBAdaptiveSOM);
  }//GEN-LAST:event_adaptiveSOMLabelMouseClicked

  private void fuzzyAdaptiveSOMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_fuzzyAdaptiveSOMLabelMouseClicked
  {//GEN-HEADEREND:event_fuzzyAdaptiveSOMLabelMouseClicked
    openFileContent(Configuration.LBFuzzyAdaptiveSOM);
  }//GEN-LAST:event_fuzzyAdaptiveSOMLabelMouseClicked

  private void T2FGMMUMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_T2FGMMUMLabelMouseClicked
  {//GEN-HEADEREND:event_T2FGMMUMLabelMouseClicked
    openFileContent(Configuration.T2FGMM_UM);
  }//GEN-LAST:event_T2FGMMUMLabelMouseClicked

  private void T2FGMMUVLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_T2FGMMUVLabelMouseClicked
  {//GEN-HEADEREND:event_T2FGMMUVLabelMouseClicked
    openFileContent(Configuration.T2FGMM_UV);
  }//GEN-LAST:event_T2FGMMUVLabelMouseClicked

  private void multiLayerBGSLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_multiLayerBGSLabelMouseClicked
  {//GEN-HEADEREND:event_multiLayerBGSLabelMouseClicked
    openFileContent(Configuration.MultiLayerBGS);
  }//GEN-LAST:event_multiLayerBGSLabelMouseClicked

  private void foregroundMaskAnalysisLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_foregroundMaskAnalysisLabelMouseClicked
  {//GEN-HEADEREND:event_foregroundMaskAnalysisLabelMouseClicked
    openFileContent(Configuration.ForegroundMaskAnalysis);
  }//GEN-LAST:event_foregroundMaskAnalysisLabelMouseClicked

  private void loadConfigComboBoxItemStateChanged(java.awt.event.ItemEvent evt)//GEN-FIRST:event_loadConfigComboBoxItemStateChanged
  {//GEN-HEADEREND:event_loadConfigComboBoxItemStateChanged
    update();
  }//GEN-LAST:event_loadConfigComboBoxItemStateChanged

  private void editButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_editButtonActionPerformed
  {//GEN-HEADEREND:event_editButtonActionPerformed
    if(loadConfigComboBox.getSelectedIndex() > 0)
      openFileContent(loadConfigComboBox.getSelectedItem().toString());
  }//GEN-LAST:event_editButtonActionPerformed

  private void runButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_runButtonActionPerformed
  {//GEN-HEADEREND:event_runButtonActionPerformed
    appTabbedPaneSelectedIndex = 0;
    execute();
  }//GEN-LAST:event_runButtonActionPerformed

  private void clearButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_clearButtonActionPerformed
  {//GEN-HEADEREND:event_clearButtonActionPerformed
    logTextPane.setText("");
  }//GEN-LAST:event_clearButtonActionPerformed

  private void autoscrollCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_autoscrollCheckBoxActionPerformed
  {//GEN-HEADEREND:event_autoscrollCheckBoxActionPerformed
    autoscroll();
  }//GEN-LAST:event_autoscrollCheckBoxActionPerformed

  private void autoscrollLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_autoscrollLabelMouseClicked
  {//GEN-HEADEREND:event_autoscrollLabelMouseClicked
    autoscrollCheckBox.setSelected(!autoscrollCheckBox.isSelected());
    autoscrollCheckBoxActionPerformed(null);
  }//GEN-LAST:event_autoscrollLabelMouseClicked

  private void checkAllButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_checkAllButtonActionPerformed
  {//GEN-HEADEREND:event_checkAllButtonActionPerformed
    enableAllBGS();
  }//GEN-LAST:event_checkAllButtonActionPerformed

  private void uncheckAllButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_uncheckAllButtonActionPerformed
  {//GEN-HEADEREND:event_uncheckAllButtonActionPerformed
    disableAllBGS();
  }//GEN-LAST:event_uncheckAllButtonActionPerformed

  private void openConfigButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_openConfigButtonActionPerformed
  {//GEN-HEADEREND:event_openConfigButtonActionPerformed
    openFileContent(Configuration.FrameProcessor);
  }//GEN-LAST:event_openConfigButtonActionPerformed

  private void fuzzySugenoIntegralLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_fuzzySugenoIntegralLabelMouseClicked
  {//GEN-HEADEREND:event_fuzzySugenoIntegralLabelMouseClicked
    openFileContent(Configuration.FuzzySugenoIntegral);
  }//GEN-LAST:event_fuzzySugenoIntegralLabelMouseClicked

  private void fuzzySugenoIntegralCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_fuzzySugenoIntegralCheckBoxActionPerformed
  {//GEN-HEADEREND:event_fuzzySugenoIntegralCheckBoxActionPerformed
    changeParam("enableFuzzySugenoIntegral", fuzzySugenoIntegralCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_fuzzySugenoIntegralCheckBoxActionPerformed

  private void fuzzyChoquetIntegralLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_fuzzyChoquetIntegralLabelMouseClicked
  {//GEN-HEADEREND:event_fuzzyChoquetIntegralLabelMouseClicked
    openFileContent(Configuration.FuzzyChoquetIntegral);
  }//GEN-LAST:event_fuzzyChoquetIntegralLabelMouseClicked

  private void fuzzyChoquetIntegralCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_fuzzyChoquetIntegralCheckBoxActionPerformed
  {//GEN-HEADEREND:event_fuzzyChoquetIntegralCheckBoxActionPerformed
    changeParam("enableFuzzyChoquetIntegral", fuzzyChoquetIntegralCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_fuzzyChoquetIntegralCheckBoxActionPerformed

  private void runButton2ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_runButton2ActionPerformed
  {//GEN-HEADEREND:event_runButton2ActionPerformed
    appTabbedPaneSelectedIndex = 1; 
    execute();
  }//GEN-LAST:event_runButton2ActionPerformed

  private void runButton3ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_runButton3ActionPerformed
  {//GEN-HEADEREND:event_runButton3ActionPerformed
    appTabbedPaneSelectedIndex = 2;
    execute();
  }//GEN-LAST:event_runButton3ActionPerformed

  private void runButton4ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_runButton4ActionPerformed
  {//GEN-HEADEREND:event_runButton4ActionPerformed
    execute();
  }//GEN-LAST:event_runButton4ActionPerformed

  private void returnButton2ActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_returnButton2ActionPerformed
  {//GEN-HEADEREND:event_returnButton2ActionPerformed
    retur(false);
  }//GEN-LAST:event_returnButton2ActionPerformed

  private void autoclearLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_autoclearLabelMouseClicked
  {//GEN-HEADEREND:event_autoclearLabelMouseClicked
    autoclearCheckBox.setSelected(!autoclearCheckBox.isSelected());
  }//GEN-LAST:event_autoclearLabelMouseClicked

  private void T2FMRFUMLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_T2FMRFUMLabelMouseClicked
  {//GEN-HEADEREND:event_T2FMRFUMLabelMouseClicked
    openFileContent(Configuration.T2FMRF_UM);
  }//GEN-LAST:event_T2FMRFUMLabelMouseClicked

  private void T2FMRFUMCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_T2FMRFUMCheckBoxActionPerformed
  {//GEN-HEADEREND:event_T2FMRFUMCheckBoxActionPerformed
    changeParam("enableT2FMRF_UM", T2FMRFUMCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_T2FMRFUMCheckBoxActionPerformed

  private void T2FMRFUVLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_T2FMRFUVLabelMouseClicked
  {//GEN-HEADEREND:event_T2FMRFUVLabelMouseClicked
    openFileContent(Configuration.T2FMRF_UV);
  }//GEN-LAST:event_T2FMRFUVLabelMouseClicked

  private void T2FMRFUVCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_T2FMRFUVCheckBoxActionPerformed
  {//GEN-HEADEREND:event_T2FMRFUVCheckBoxActionPerformed
    changeParam("enableT2FMRF_UV", T2FMRFUVCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_T2FMRFUVCheckBoxActionPerformed

  private void GMGCheckBoxActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_GMGCheckBoxActionPerformed
  {//GEN-HEADEREND:event_GMGCheckBoxActionPerformed
    changeParam("enableGMG", GMGCheckBox, Configuration.FrameProcessor);
  }//GEN-LAST:event_GMGCheckBoxActionPerformed

  private void GMGLabelMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_GMGLabelMouseClicked
  {//GEN-HEADEREND:event_GMGLabelMouseClicked
    openFileContent(Configuration.GMG);
  }//GEN-LAST:event_GMGLabelMouseClicked

    private void VuMeterLabelMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_VuMeterLabelMouseClicked
      openFileContent(Configuration.VuMeter);
    }//GEN-LAST:event_VuMeterLabelMouseClicked

    private void VuMeterCheckBoxActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_VuMeterCheckBoxActionPerformed
      changeParam("enableVuMeter", VuMeterCheckBox, Configuration.FrameProcessor);
    }//GEN-LAST:event_VuMeterCheckBoxActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JCheckBox GMGCheckBox;
  private javax.swing.JLabel GMGLabel;
  private javax.swing.JCheckBox ROIdefinedCheckBox;
  private javax.swing.JCheckBox T2FGMMUMCheckBox;
  private javax.swing.JLabel T2FGMMUMLabel;
  private javax.swing.JCheckBox T2FGMMUVCheckBox;
  private javax.swing.JLabel T2FGMMUVLabel;
  private javax.swing.JCheckBox T2FMRFUMCheckBox;
  private javax.swing.JLabel T2FMRFUMLabel;
  private javax.swing.JCheckBox T2FMRFUVCheckBox;
  private javax.swing.JLabel T2FMRFUVLabel;
  private javax.swing.JCheckBox VuMeterCheckBox;
  private javax.swing.JLabel VuMeterLabel;
  private javax.swing.JCheckBox adaptiveBackgroundLearningCheckBox;
  private javax.swing.JLabel adaptiveBackgroundLearningLabel;
  private javax.swing.JCheckBox adaptiveMedianCheckBox;
  private javax.swing.JLabel adaptiveMedianLabel;
  private javax.swing.JCheckBox adaptiveSOMCheckBox;
  private javax.swing.JLabel adaptiveSOMLabel;
  private javax.swing.JTabbedPane appTabbedPane;
  private javax.swing.JCheckBox autoclearCheckBox;
  private javax.swing.JLabel autoclearLabel;
  private javax.swing.JCheckBox autoscrollCheckBox;
  private javax.swing.JLabel autoscrollLabel;
  private javax.swing.JTextField cameraIndexTextField;
  private javax.swing.JButton checkAllButton;
  private javax.swing.JButton clearButton;
  private javax.swing.JTextField cmdTextField;
  private javax.swing.JTextField compareAtTextField;
  private javax.swing.JButton editButton;
  private javax.swing.JCheckBox eigenbackgroundCheckBox;
  private javax.swing.JLabel eigenbackgroundLabel;
  private javax.swing.JTextPane fileContentTextPane;
  private javax.swing.JLabel fileNameLabel;
  private javax.swing.JCheckBox foregroundMaskAnalysisCheckBox;
  private javax.swing.JLabel foregroundMaskAnalysisLabel;
  private javax.swing.JCheckBox frameDifferenceCheckBox;
  private javax.swing.JLabel frameDifferenceLabel;
  private javax.swing.JCheckBox fuzzyAdaptiveSOMCheckBox;
  private javax.swing.JLabel fuzzyAdaptiveSOMLabel;
  private javax.swing.JCheckBox fuzzyChoquetIntegralCheckBox;
  private javax.swing.JLabel fuzzyChoquetIntegralLabel;
  private javax.swing.JCheckBox fuzzyGaussianCheckBox;
  private javax.swing.JLabel fuzzyGaussianLabel;
  private javax.swing.JCheckBox fuzzySugenoIntegralCheckBox;
  private javax.swing.JLabel fuzzySugenoIntegralLabel;
  private javax.swing.JCheckBox gaussianAverageCheckBox;
  private javax.swing.JLabel gaussianAverageLabel;
  private javax.swing.JCheckBox grimsonGMMCheckBox;
  private javax.swing.JLabel grimsonGMMLabel;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel10;
  private javax.swing.JLabel jLabel11;
  private javax.swing.JLabel jLabel12;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JLabel jLabel34;
  private javax.swing.JLabel jLabel4;
  private javax.swing.JLabel jLabel5;
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JLabel jLabel8;
  private javax.swing.JLabel jLabel9;
  private javax.swing.JMenu jMenu1;
  private javax.swing.JMenu jMenu2;
  private javax.swing.JMenu jMenu3;
  private javax.swing.JMenu jMenu4;
  private javax.swing.JMenuBar jMenuBar1;
  private javax.swing.JMenuBar jMenuBar2;
  private javax.swing.JMenuItem jMenuItem1;
  private javax.swing.JMenuItem jMenuItem2;
  private javax.swing.JPanel jPanel1;
  private javax.swing.JPanel jPanel10;
  private javax.swing.JPanel jPanel11;
  private javax.swing.JPanel jPanel12;
  private javax.swing.JPanel jPanel13;
  private javax.swing.JPanel jPanel16;
  private javax.swing.JPanel jPanel17;
  private javax.swing.JPanel jPanel18;
  private javax.swing.JPanel jPanel2;
  private javax.swing.JPanel jPanel3;
  private javax.swing.JPanel jPanel4;
  private javax.swing.JPanel jPanel6;
  private javax.swing.JPanel jPanel7;
  private javax.swing.JPanel jPanel9;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JToolBar.Separator jSeparator1;
  private javax.swing.JToolBar.Separator jSeparator2;
  private javax.swing.JToolBar.Separator jSeparator3;
  private javax.swing.JToolBar.Separator jSeparator4;
  private javax.swing.JToolBar.Separator jSeparator5;
  private javax.swing.JToolBar.Separator jSeparator6;
  private javax.swing.JToolBar jToolBar1;
  private javax.swing.JToolBar jToolBar2;
  private javax.swing.JComboBox loadConfigComboBox;
  private javax.swing.JTextPane logTextPane;
  private javax.swing.JScrollPane logTextScrollPane;
  private javax.swing.JCheckBox mixtureOfGaussiansCheckBox;
  private javax.swing.JLabel mixtureOfGaussiansLabel;
  private javax.swing.JCheckBox multiLayerBGSCheckBox;
  private javax.swing.JLabel multiLayerBGSLabel;
  private javax.swing.JCheckBox openCVMoGV1CheckBox;
  private javax.swing.JLabel openCVMoGV1Label;
  private javax.swing.JCheckBox openCVMoGV2CheckBox;
  private javax.swing.JLabel openCVMoGV2Label;
  private javax.swing.JButton openConfigButton;
  private javax.swing.JCheckBox preProcessorCheckBox;
  private javax.swing.JLabel preProcessorLabel;
  private javax.swing.JButton reloadButton;
  private javax.swing.JTextField resizeFrameTextField;
  private javax.swing.JButton returnButton;
  private javax.swing.JButton returnButton2;
  private javax.swing.JButton runButton;
  private javax.swing.JButton runButton2;
  private javax.swing.JButton runButton3;
  private javax.swing.JButton runButton4;
  private javax.swing.JButton saveButton;
  private javax.swing.JButton setDatasetFolderButton;
  private javax.swing.JButton setImageRefButton;
  private javax.swing.JButton setInputFileButton;
  private javax.swing.JButton setTestFileButton;
  private javax.swing.JButton setTrainFileButton;
  private javax.swing.JCheckBox showOutputCheckBox;
  private javax.swing.JCheckBox simpleGaussianCheckBox;
  private javax.swing.JLabel simpleGaussianLabel;
  private javax.swing.JCheckBox staticFrameDifferenceCheckBox;
  private javax.swing.JLabel staticFrameDifferenceLabel;
  private javax.swing.JTextField stopVideoAtTextField;
  private javax.swing.JCheckBox temporalMeanCheckBox;
  private javax.swing.JLabel temporalMeanLabel;
  private javax.swing.JCheckBox temporalMedianCheckBox;
  private javax.swing.JLabel temporalMedianLabel;
  private javax.swing.JComboBox tictocComboBox;
  private javax.swing.JButton uncheckAllButton;
  private javax.swing.JRadioButton useCameraRadioButton;
  private javax.swing.JRadioButton useDatasetRadioButton;
  private javax.swing.JCheckBox useForegroundMaskAnalysisCheckBox;
  private javax.swing.JCheckBox useROICheckBox;
  private javax.swing.JRadioButton useVideoRadioButton;
  private javax.swing.JCheckBox verticalFlipCheckBox;
  private javax.swing.JCheckBox weightedMovingMeanCheckBox;
  private javax.swing.JLabel weightedMovingMeanLabel;
  private javax.swing.JCheckBox weightedMovingVarianceCheckBox;
  private javax.swing.JLabel weightedMovingVarianceLabel;
  private javax.swing.JTextField x0ROITextField;
  private javax.swing.JTextField x1ROITextField;
  private javax.swing.JTextField y0ROITextField;
  private javax.swing.JTextField y1ROITextField;
  private javax.swing.JCheckBox zivkovicGMMCheckBox;
  private javax.swing.JLabel zivkovicGMMLabel;
  // End of variables declaration//GEN-END:variables
}

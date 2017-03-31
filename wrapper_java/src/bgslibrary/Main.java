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
package bgslibrary;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.InputStream;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public final class Main
{
  static
  {
    System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
  }
  
  private static BgsLib bgslib;
  
  private JFrame window;
  private JButton startButton, stopButton;
  private ImagePanel image;
  private JComboBox comboBox;
  private String selectedAlgorithm;

  public Main()
  {
    buildGUI();
  }

  private void buildGUI()
  {
    window = new JFrame("Camera Panel");

    startButton = new JButton("Start");
    stopButton = new JButton("Stop");

    window.add(startButton, BorderLayout.WEST);
    window.add(stopButton, BorderLayout.EAST);

    //image = new ImagePanel(new ImageIcon("images/320x240.gif").getImage());
    image = new ImagePanel(new ImageIcon("images/640x480.png").getImage());
    window.add(image, BorderLayout.CENTER);

    String[] options = {"* Select a background subtraction algorithm"};
    options = Utils.generalConcatAll(options, bgslib.getListAlgorithms());
    
    comboBox = new JComboBox(options);
    comboBox.addActionListener(new ActionListener()
    {

      @Override
      public void actionPerformed(ActionEvent e)
      {
        if(begin == true)
        {
          JOptionPane.showMessageDialog(null, "Stop capture first.", "Warning",
          JOptionPane.WARNING_MESSAGE);
          comboBox.setSelectedIndex(0);
          selectedAlgorithm = null;
          return;
        }
        
        String item = comboBox.getSelectedItem().toString();
        
        if(item.startsWith("*"))
          selectedAlgorithm = null;
        else
          selectedAlgorithm = item;
        
        //System.out.println("Selected algorithm: " + selectedAlgorithm);
      }
    });
    window.add(comboBox, BorderLayout.SOUTH);

    window.setSize(default_width + 130, default_height + 28);
    window.setLocationRelativeTo(null);
    window.setVisible(true);
    window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    window.setResizable(false);

    startButton.addActionListener((ActionEvent e) ->
    {
      start();
    });

    stopButton.addActionListener((ActionEvent e) ->
    {
      stop();
    });
  }

  private Boolean begin = false;
  private VideoCapture video = null;
  private CaptureThread thread = null;

  private void start()
  {
    if (begin == false)
    {
      video = new VideoCapture(0);

      if (video.isOpened())
      {
        if(selectedAlgorithm != null)
          BgsLib.constructObject(selectedAlgorithm);
        
        thread = new CaptureThread();
        thread.start();
        begin = true;
      }
    }
  }

  private MatOfByte matOfByte = new MatOfByte();
  private BufferedImage bufImage = null;
  private InputStream in;
  private final int default_width = 640;
  private final int default_height = 480;
  private Mat frameaux = new Mat();
  private Mat frame = new Mat(default_height, default_width, CvType.CV_8UC3);
  private Mat fgmask = new Mat();

  class CaptureThread extends Thread
  {
    @Override
    public void run()
    {
      if (video.isOpened())
      {
        while (begin == true)
        {
          video.retrieve(frameaux);
          Imgproc.resize(frameaux, frame, frame.size());
          
          if(selectedAlgorithm != null)
          {
            BgsLib.computeForegroundMask(frame.getNativeObjAddr(), fgmask.getNativeObjAddr());
            bufImage = Utils.toBufferedImage(fgmask);
          }
          else
            bufImage = Utils.toBufferedImage(frame);
          
          image.updateImage(bufImage);

          try
          {
            Thread.sleep(5);
          }
          catch (Exception ex)
          {
          }
        }
      }
    }
  }

  private void stop()
  {
    begin = false;
    try
    {
      Thread.sleep(500);
    }
    catch (Exception ex)
    {
    }
    
    if(video != null)
    {
      video.release();
      video = null;
    }
    
    BgsLib.destroyObject();
  }

  public static void main(String[] args)
  {
    new Main();
  }
}

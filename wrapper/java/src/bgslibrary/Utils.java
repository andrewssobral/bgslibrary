package bgslibrary;

import java.awt.FlowLayout;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.UUID;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Utils
{
  static final private SecureRandom random = new SecureRandom();

  static final public String nextSessionId()
  {
    return new BigInteger(130, random).toString(32);
  }

  static final public String nextUUID()
  {
    return UUID.randomUUID().toString();
  }

  static final public BufferedImage toBufferedImage(Mat m)
  {
    int type = BufferedImage.TYPE_BYTE_GRAY;
    if (m.channels() > 1)
      type = BufferedImage.TYPE_3BYTE_BGR;
    int bufferSize = m.channels() * m.cols() * m.rows();
    byte[] b = new byte[bufferSize];
    m.get(0, 0, b); // get all the pixels
    BufferedImage image = new BufferedImage(m.cols(), m.rows(), type);
    final byte[] targetPixels = ((DataBufferByte) image.getRaster().getDataBuffer()).getData();
    System.arraycopy(b, 0, targetPixels, 0, b.length);
    return image;
  }

  static final public void imshow(Mat image)
  {
    try
    {
      BufferedImage bufImage = toBufferedImage(image);
      JFrame frame = new JFrame("Image");
      frame.getContentPane().setLayout(new FlowLayout());
      frame.getContentPane().add(new JLabel(new ImageIcon(bufImage)));
      frame.pack();
      frame.setSize(image.width() + 40, image.height() + 60);
      frame.setLocationRelativeTo(null);
      frame.setVisible(true);
      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

  static final public void writeImage(Mat image, String fileName)
  {
    //System.out.println(String.format("Writing image at %s", fileName));
    Imgcodecs.imwrite(fileName, image);
  }

  static final public String[] generalConcatAll(String[]... jobs)
  {
    int len = 0;
    for (final String[] job : jobs)
      len += job.length;

    final String[] result = new String[len];

    int currentPos = 0;
    for (final String[] job : jobs)
    {
      System.arraycopy(job, 0, result, currentPos, job.length);
      currentPos += job.length;
    }

    return result;
  }
}

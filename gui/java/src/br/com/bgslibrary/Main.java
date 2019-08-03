package br.com.bgslibrary;

import br.com.bgslibrary.gui.MainFrame;

public class Main
{
  public static void main(String args[])
  {
    try
    {
      if(System.getProperty("os.name").toLowerCase().contains("windows"))
        javax.swing.UIManager.setLookAndFeel(new com.sun.java.swing.plaf.windows.WindowsLookAndFeel());
      /*
      for(javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels())
      {
        //System.out.println("LookAndFeel: " + info.getName());
        if("Nimbus".equals(info.getName()))
        {
          javax.swing.UIManager.setLookAndFeel(info.getClassName());
          break;
        }
      }
      */
    }
    catch(Exception ex)
    {
      java.util.logging.Logger.getLogger(MainFrame.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    }
    
    java.awt.EventQueue.invokeLater(new Runnable()
    {
      @Override
      public void run()
      {
        new MainFrame().setVisible(true);
      }
    });
  }
}

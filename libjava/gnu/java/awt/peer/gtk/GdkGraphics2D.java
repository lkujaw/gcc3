/* GdkGraphics2D.java
   Copyright (C) 2003 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package gnu.java.awt.peer.gtk;

import java.awt.*;
import java.awt.geom.*;
import java.awt.font.*;
import java.awt.color.*;
import java.awt.image.*;
import java.awt.image.renderable.*;

import java.text.AttributedCharacterIterator;
import java.util.Map;
import java.lang.Integer;
import gnu.classpath.Configuration;

public class GdkGraphics2D extends Graphics2D
{

  //////////////////////////////////////
  ////// State Management Methods //////
  //////////////////////////////////////

  static 
  {
    if (Configuration.INIT_LOAD_LIBRARY)
      {
        System.loadLibrary("gtkpeer");
      }
    initStaticState ();
  }
  native static void initStaticState ();
  private final int native_state = GtkGenericPeer.getUniqueInteger();  

  private Paint paint;
  private Stroke stroke;
  private Color fg;
  private Color bg;
  private Shape clip;
  private AffineTransform transform;
  private GtkComponentPeer component;
  private GdkFont font;  

  native private int[] initState (GtkComponentPeer component);
  native private void initState (int width, int height);
  native private void copyState (GdkGraphics2D g);
  native public void dispose ();

  public void finalize ()
  {
    dispose();
  }

  public Graphics create ()
  {
    return new GdkGraphics2D (this);
  }

  public Graphics create (int x, int y, int width, int height)
  {
    return new GdkGraphics2D (width, height);
  }

  GdkGraphics2D (GdkGraphics2D g)
  {
    paint = g.paint;
    stroke = g.stroke;

    if (g.fg.getAlpha() != -1)
      fg = new Color (g.fg.getRed (), g.fg.getGreen (), 
                      g.fg.getBlue (), g.fg.getAlpha ());
    else 
      fg = new Color (g.fg.getRGB ());

    if (g.bg.getAlpha() != -1)
      bg = new Color(g.bg.getRed (), g.bg.getGreen (), 
                     g.bg.getBlue (), g.bg.getAlpha ());
    else
      bg = new Color (g.bg.getRGB ());

    if (g.clip == null)
      clip = null;
    else
      clip = new Rectangle (g.getClipBounds ());

    if (g.transform == null)
      transform = null;
    else
      transform = new AffineTransform (g.transform);

    component = g.component;
    copyState (g);

    setColor (fg);
    setClip (clip);
    setTransform (transform);
  }

  GdkGraphics2D (int width, int height)
  {
    initState (width, height);
    bg = Color.black;
    fg = Color.black;
    transform = new AffineTransform ();
  }

  GdkGraphics2D (GtkComponentPeer component)
  {
    this.component = component;
    int rgb[] = initState (component);
    fg = new Color (rgb[0], rgb[1], rgb[2]);
    bg = new Color (rgb[3], rgb[4], rgb[5]);
    transform = new AffineTransform ();
  }


  ////////////////////////////////////
  ////// Native Drawing Methods //////
  ////////////////////////////////////

  // GDK drawing methods
  private native void gdkDrawDrawable (GdkGraphics2D other, int x, int y);

  // drawing utility methods
  private native void drawPixels (int pixels[], int w, int h, int stride);
  private native void setTexturePixels (int pixels[], int w, int h, int stride);
  private native void setGradient (double x1, double y1,
                                   double x2, double y2,
                                   int r1, int g1, int b1, int a1,
                                   int r2, int g2, int b2, int a2,
                                   boolean cyclic);

  // simple passthroughs to cairo
  private native void cairoSave ();
  private native void cairoRestore ();
  private native void cairoSetMatrix (double m00, double m10, 
                                      double m01, double m11,
                                      double m02, double m12);
  private native void cairoSetOperator (int cairoOperator);
  private native void cairoSetRGBColor (double red, double green, double blue);
  private native void cairoSetAlpha (double alpha);
  private native void cairoSetFillRule (int cairoFillRule);
  private native void cairoSetLineWidth (double width);
  private native void cairoSetLineCap (int cairoLineCap);
  private native void cairoSetLineJoin (int cairoLineJoin);
  private native void cairoSetDash (double dashes[], int ndash, double offset);
  private native void cairoSetMiterLimit (double limit);
  private native void cairoTranslate (double tx, double ty);
  private native void cairoScale (double sx, double sy);
  private native void cairoRotate (double angle);
  private native void cairoNewPath ();
  private native void cairoMoveTo (double x, double y);
  private native void cairoLineTo (double x, double y);
  private native void cairoCurveTo (double x1, double y1,
                                 double x2, double y2,
                                 double x3, double y3);  
  private native void cairoRelMoveTo (double dx, double dy);
  private native void cairoRelLineTo (double dx, double dy);
  private native void cairoRelCurveTo (double dx1, double dy1,
                                    double dx2, double dy2,
                                    double dx3, double dy3);
  private native void cairoRectangle (double x, double y, 
                                   double width, double height);
  private native void cairoClosePath ();
  private native void cairoStroke ();
  private native void cairoFill ();
  private native void cairoClip ();


  /////////////////////////////////////////////
  ////// General Drawing Support Methods //////
  /////////////////////////////////////////////

  double x;
  double y;
  private void setPos (double nx, double ny)
  {
    x = nx;
    y = ny;
  }

  private void walkPath(PathIterator p)
  {
    double coords[] = new double[6];

    cairoSetFillRule (p.getWindingRule ());
    for ( ; ! p.isDone (); p.next())
      {
        int seg = p.currentSegment (coords);
        switch(seg)
          {

          case PathIterator.SEG_MOVETO:
            setPos(coords[0], coords[1]);
            cairoMoveTo (coords[0], coords[1]);
            break;

          case PathIterator.SEG_LINETO:
            setPos(coords[0], coords[1]);
            cairoLineTo (coords[0], coords[1]);
            break;

          case PathIterator.SEG_QUADTO:

            // splitting a quadratic bezier into a cubic:
            // see: http://pfaedit.sourceforge.net/bezier.html

            double x1 = x + (2.0/3.0) * (coords[0] - x);
            double y1 = y + (2.0/3.0) * (coords[1] - y);
            
            double x2 = x1 + (1.0/3.0) * (coords[2] - x);
            double y2 = y1 + (1.0/3.0) * (coords[3] - y);

            setPos(coords[2], coords[3]);
            cairoCurveTo (x1, y1,
                          x2, y2,
                          coords[2], coords[3]);
            break;

          case PathIterator.SEG_CUBICTO:
            setPos(coords[4], coords[5]);
            cairoCurveTo (coords[0], coords[1],
                          coords[2], coords[3],
                          coords[4], coords[5]);
            break;

          case PathIterator.SEG_CLOSE:
            cairoClosePath ();
            break;
          }
      }    
  }


  //////////////////////////////////////////////////
  ////// Implementation of Graphics2D Methods //////
  //////////////////////////////////////////////////

  public void draw (Shape s)
  {

    if (stroke != null &&
        !(stroke instanceof BasicStroke))
      {
        fill (stroke.createStrokedShape (s));
        return;
      }

    cairoSave ();
    cairoNewPath ();
    if (s instanceof Rectangle2D)
      {
        Rectangle2D r = (Rectangle2D)s;
        cairoRectangle (r.getX (), r.getY (), r.getWidth (), r.getHeight ());
      }
    else      
      walkPath (s.getPathIterator (null));
    cairoStroke ();
    cairoRestore ();
  }

  public void fill(Shape s)
  {
    cairoSave();
    cairoNewPath ();
    if (s instanceof Rectangle2D)
      {
        Rectangle2D r = (Rectangle2D)s;
        cairoRectangle (r.getX (), r.getY (), r.getWidth (), r.getHeight ());
      }
    else      
      walkPath (s.getPathIterator (null));
    cairoFill ();
    cairoRestore ();
  }

  public void clip (Shape s)
  {
    clip = s;
    cairoNewPath ();
    if (s instanceof Rectangle2D)
      {
        Rectangle2D r = (Rectangle2D)s;
        cairoRectangle (r.getX (), r.getY (), 
                        r.getWidth (), r.getHeight ());
      }
    else      
      walkPath (s.getPathIterator (null));
    cairoClosePath ();
    cairoClip ();
  }

  public Paint getPaint ()
  {
    return paint;
  }

  public AffineTransform getTransform ()
  {
    return transform;
  }

  public void setPaint (Paint p)
  {
    paint = p;
    if (paint instanceof Color)
      {
        setColor ((Color) paint);
      }
    else if (paint instanceof TexturePaint)
      {
        TexturePaint tp = (TexturePaint) paint;
        BufferedImage img = tp.getImage ();
        int pixels[] = img.getRGB(0, 0, img.getWidth (), 
                                  img.getHeight (), null, 
                                  0, img.getWidth ());
        setTexturePixels (pixels, img.getWidth (), 
                          img.getHeight (), img.getWidth ());
      }
    else if (paint instanceof GradientPaint)
      {
        GradientPaint gp = (GradientPaint) paint;
        Point2D p1 = gp.getPoint1 ();
        Point2D p2 = gp.getPoint2 ();
        Color c1 = gp.getColor1 ();
        Color c2 = gp.getColor2 ();        
        setGradient (p1.getX (), p1.getY (),
                     p2.getX (), p2.getY (),
                     c1.getRed (), c1.getGreen (), 
                     c1.getBlue (), c1.getAlpha (),
                     c2.getRed (), c2.getGreen (), 
                     c2.getBlue (), c2.getAlpha (),
                     gp.isCyclic ());
      }
    else
      throw new java.lang.UnsupportedOperationException ();
  }

  public void setTransform (AffineTransform tx)
  {
    transform = tx;
    if (transform != null)
      {
        double m[] = new double[6];
        transform.getMatrix (m);
        cairoSetMatrix (m[0], m[1], m[2], m[3], m[4], m[5]);
      }
  }

  public void transform (AffineTransform tx)
  {
    if (transform == null)
      transform = new AffineTransform (tx);
    else
      transform.concatenate (tx);
    setTransform (transform);
  }

  public void rotate(double theta)
  {
    if (transform != null)
      transform.rotate (theta);
    cairoRotate (theta);
  }

  public void rotate(double theta, double x, double y)
  {
    if (transform != null)
      transform.rotate (theta, x, y);
    cairoTranslate (x, y);
    cairoRotate (theta);
    cairoTranslate (-x, -y);
  }

  public void scale(double sx, double sy)
  {
    if (transform != null)
      transform.scale (sx, sy);
    cairoScale (sx, sy);
  }

  public void translate (double tx, double ty)
  {
    if (transform != null)
      transform.translate (tx, ty);
    cairoTranslate (tx, ty);
  }

  public void translate (int x, int y)
  {
    translate ((double) x, (double) y);
  }

  public Stroke getStroke()
  {
    return stroke;
  }

  public void setStroke (Stroke st)
  {
    stroke = st;
    if (stroke instanceof BasicStroke)
      {
        BasicStroke bs = (BasicStroke) stroke;
        cairoSetLineCap (bs.getEndCap());
        cairoSetLineWidth (bs.getLineWidth());
        cairoSetLineJoin (bs.getLineJoin());
        cairoSetMiterLimit (bs.getMiterLimit());
        float dashes[] = bs.getDashArray();
        if (dashes != null)
          {
            double double_dashes[] = new double[dashes.length];
            for (int i = 0; i < dashes.length; i++)
              double_dashes[i] = dashes[i];
            cairoSetDash (double_dashes, double_dashes.length, 
                          (double) bs.getDashPhase ());        
          }
      }
  }


  ////////////////////////////////////////////////
  ////// Implementation of Graphics Methods //////
  ////////////////////////////////////////////////

  public void setPaintMode () 
  { 
    setComposite (java.awt.AlphaComposite.Xor); 
  }

  public void setXORMode (Color c) 
  { 
    setComposite (new BitwiseXorComposite (c)); 
  }

  public void setColor (Color c)
  {
    fg = c;
    cairoSetRGBColor (fg.getRed() / 255.0, 
                      fg.getGreen() / 255.0, 
                      fg.getBlue() / 255.0);
    cairoSetAlpha ((fg.getAlpha() & 255) / 255.0);
  }

  public Color getColor ()
  {
    return fg;
  }

  public void clipRect (int x, int y, int width, int height)
  {
    // this is *slightly* different than all the other clip functions: it
    // intersects the clip area with the new clip rectangle. obviously.  of
    // course, since Shape doesn't *have* any way of intersecting with a
    // rectangle, we will promote the current clipping region to its
    // bounding rectangle and then intersect with that.
    if (clip == null)
      {
        cairoNewPath ();
        cairoRectangle (x, y, width, height);
        cairoClosePath ();
        cairoClip ();
        clip = new Rectangle (x, y, width, height);
      }
    else
      {
        clip (clip.getBounds ().intersection 
              (new Rectangle (x, y, width, height)));
      }
  }

  public Shape getClip ()
  {
    return clip;
  }

  public Rectangle getClipBounds ()
  {
    if (clip == null)
      return null;
    else
      return clip.getBounds ();
  }

  public void setClip (int x, int y, int width, int height)
  {
    cairoNewPath ();
    cairoRectangle (x, y, width, height);
    cairoClosePath ();
    cairoClip ();
    clip = new Rectangle (x, y, width, height);
  }

  public void setClip (Shape s)
  {
    clip (s);
  }

  public void draw3DRect(int x, int y, int width, 
                         int height, boolean raised)
  {
    Color std = fg;
    Color light = std.brighter();
    Color dark = std.darker();

    if (!raised)
      {
        Color t = light;
        light = dark;
        dark = t;
      }
    
    double x1 = (double) x;
    double x2 = (double) x + width;

    double y1 = (double) y;
    double y2 = (double) y + height;

    cairoSave ();
    
    cairoNewPath ();
    setColor (light);
    cairoMoveTo (x1, y1);
    cairoLineTo (x2, y1);
    cairoLineTo (x2, y2);
    cairoStroke ();
    
    cairoNewPath ();
    setColor (dark);
    cairoMoveTo (x1, y1);
    cairoLineTo (x1, y2);
    cairoLineTo (x2, y2);
    cairoStroke ();
    
    cairoRestore ();    
    setColor (std);

  }

  public void fill3DRect(int x, int y, int width, 
                         int height, boolean raised)
  {
    double step = 1.0;
    if (stroke != null && stroke instanceof BasicStroke)
      {
        BasicStroke bs = (BasicStroke) stroke;
        step = bs.getLineWidth();
      }

    Color bright = fg.brighter ();
    Color dark = fg.darker ();
      
    draw3DRect (x, y, width, height, raised);
    
    cairoSave ();
    cairoTranslate (step/2.0, step/2.0);
    cairoNewPath ();
    cairoRectangle ((double) x, (double) y, 
                    ((double) width) - step, 
                    ((double) height) - step );
    cairoClosePath ();
    cairoFill ();
    cairoRestore ();
  }


  public void drawRect (int x, int y, int width, int height)
  {
    draw(new Rectangle (x, y, width, height));
  }

  public void fillRect (int x, int y, int width, int height)
  {
    fill(new Rectangle (x, y, width, height));
  }

  public void clearRect (int x, int y, int width, int height)
  {
    cairoSave ();
    cairoSetRGBColor (bg.getRed() / 255.0, 
                      bg.getGreen() / 255.0, 
                      bg.getBlue() / 255.0);
    cairoSetAlpha (1.0);
    cairoNewPath ();
    cairoRectangle (x, y, width, height);
    cairoClosePath ();
    cairoFill ();
    cairoRestore ();
  }

  public void setBackground(Color c)
  {
    bg = c;
  }


  public Color getBackground()
  {
    return bg;
  }


  private void doPolygon(int[] xPoints, int[] yPoints, int nPoints, 
                         boolean close, boolean fill)
  {    
    if (nPoints < 1)
      return;
    GeneralPath gp = new GeneralPath ();
    gp.moveTo ((float)xPoints[0], (float)yPoints[0]);
    for (int i = 1; i < nPoints; i++)
      gp.lineTo ((float)xPoints[i], (float)yPoints[i]);
    
    if (close)
      gp.closePath ();

    Shape sh = gp;
    if (fill == false &&
        stroke != null &&
        !(stroke instanceof BasicStroke))
      {
        sh = stroke.createStrokedShape (gp);
        fill = true;
      }
    
    if (fill) 
      fill (sh);
    else 
      draw (sh);
  }

  public void drawLine (int x1, int y1, int x2, int y2)
  {
    int xp[] = new int[2];
    int yp[] = new int[2];

    xp[0] = x1;
    xp[1] = x2;
    yp[0] = y1;
    yp[1] = y2;
    
    doPolygon (xp, yp, 2, false, false);
  }

  public void fillPolygon(int[] xPoints, int[] yPoints, int nPoints)
  {
    doPolygon (xPoints, yPoints, nPoints, true, true);
  }
  
  public void drawPolygon(int[] xPoints, int[] yPoints, int nPoints)
  {    
    doPolygon (xPoints, yPoints, nPoints, true, false);
  }

  public void drawPolyline(int[] xPoints, int[] yPoints, int nPoints)
  {
    doPolygon (xPoints, yPoints, nPoints, false, false);
  }

  private boolean drawRaster (ColorModel cm, Raster r)
  {
    if (r == null)
      return false;

    SampleModel sm = r.getSampleModel ();
    DataBuffer db = r.getDataBuffer ();

    if (db == null || sm == null)
      return false;

    if (cm == null)
      cm = ColorModel.getRGBdefault ();

    int pixels[] = null;

    if (sm.getDataType () == DataBuffer.TYPE_INT &&
        db instanceof DataBufferInt &&
        db.getNumBanks () == 1)
      {
        // single bank, ARGB-ints buffer in sRGB space
        DataBufferInt dbi = (DataBufferInt)db;
        pixels = dbi.getData ();
      }
    else
      pixels = r.getPixels (0, 0, r.getWidth (), r.getHeight (), pixels);
    
    ColorSpace cs = cm.getColorSpace ();
    if (cs != null && 
        cs.getType () != ColorSpace.CS_sRGB)
      {
        int pixels2[] = new int[pixels.length];        
        for (int i = 0; i < pixels2.length; i++)
          pixels2[i] = cm.getRGB (pixels[i]);        
        pixels = pixels2;
      }
    
    cairoSave ();
    cairoTranslate (x, y);
    drawPixels (pixels, r.getWidth (), r.getHeight (), r.getWidth ());
    cairoRestore ();    
    return true;
  }

  public boolean drawImage (Image img, int x, int y, 
                            ImageObserver observer)
  {
    if (img instanceof GtkOffScreenImage &&
        img.getGraphics () instanceof GdkGraphics2D &&            
        (transform == null || transform.isIdentity ())) 
      {
        // we are being asked to flush a double buffer from Gdk
        GdkGraphics2D g2 = (GdkGraphics2D) img.getGraphics ();
        gdkDrawDrawable (g2, x, y);
        return true;
      }
    else
      {
        if (img instanceof BufferedImage)
          {
            // draw an image which has actually been loaded into memory fully
            BufferedImage b = (BufferedImage) img;
            return drawRaster (b.getColorModel (), b.getData ());
          }        
        else
          {
            // begin progressive loading in a separate thread
            new PainterThread (this, img);
            return false;
          }
      }
  }


  ////////////////////////////////////////
  ////// Supporting Private Classes //////
  ////////////////////////////////////////
  
  private class PainterThread implements Runnable, ImageConsumer
  {

    // this is a helper which is spun off when someone tries to do
    // Graphics2D.drawImage on an image we cannot determine to be either
    // one of our own offscreen images or a BufferedImage; that is, when
    // someone wants to draw an image which is possibly still loading over
    // a network or something. you run it in a separate thread and it
    // writes through to the underlying Graphics2D as pixels becomg
    // available.

    GdkGraphics2D gr;
    Image image;
    ColorModel defaultModel;

    public PainterThread (GdkGraphics2D g, Image im)
    {
      image = im;
      this.gr = (GdkGraphics2D) g.create ();
      new Thread (this).start ();
    }
    
    public void imageComplete (int status)
    {
    }
    
    public void setColorModel (ColorModel model) 
    {
      defaultModel = model;
    }
    
    public void setDimensions (int width, int height)
    {
    }
    
    public void setHints (int hintflags)
    {
    }
    
    public void setPixels (int x, int y, int w, int h, ColorModel model, 
                           byte[] pixels, int off, int scansize)
    {
    }
    
    public void setPixels (int x, int y, int w, int h, ColorModel model, 
                           int[] pixels, int off, int scansize)
      {
        gr.cairoSave ();
        gr.cairoTranslate (x, y);

        if (model == null)
          model = defaultModel;

        int pixels2[];
        if (model != null)
          {
            pixels2 = new int[pixels.length];
            for (int yy = 0; yy < h; yy++)
              for (int xx = 0; xx < w; xx++)
                {
                  int i = yy * scansize + xx;
                  pixels2[i] = model.getRGB (pixels[i]);
                }
          }
        else
          pixels2 = pixels;

        gr.drawPixels (pixels2, w, h, scansize);
        gr.cairoRestore ();
      }

    public void setProperties (java.util.Hashtable props)
    {
    }
    
    public void run ()
    {
      image.getSource ().startProduction (this);
      gr.dispose ();
    }
  }


  private class BitwiseXorComposite implements Composite
  {
    // this is a special class which does a bitwise XOR composite, for
    // backwards compatibility sake. it does *not* implement the
    // porter-duff XOR operator.  the porter-duff XOR is unrelated to
    // bitwise XOR; it just happens to have a similar name but it
    // represents a desire to composite the exclusive or of overlapping
    // subpixel regions. bitwise XOR is for drawing "highlights" such as
    // cursors (in a cheap oldskool bitblit fashion) by inverting colors
    // temporarily and then inverting them back.

    Color xorColor;
    
    class BitwiseXorCompositeContext implements CompositeContext
    {
      ColorModel srcColorModel;
      ColorModel dstColorModel;
      
      public BitwiseXorCompositeContext (ColorModel s,
                                         ColorModel d)
      {
        srcColorModel = s;
        dstColorModel = d;
      }

      public void dispose () 
      {
      }

      public void compose (Raster src,
                           Raster dstIn,
                           WritableRaster dstOut)
      {
        Rectangle srcRect = src.getBounds ();
        Rectangle dstInRect = dstIn.getBounds ();
        Rectangle dstOutRect = dstOut.getBounds ();
        
        int xp = xorColor.getRGB ();
        int x = 0, y = 0;
        int w = Math.min (Math.min (srcRect.width, dstOutRect.width), dstInRect.width);
        int h = Math.min (Math.min (srcRect.height, dstOutRect.height), dstInRect.height);
        Object srcPix = null, dstPix = null;

        for (y = 0; y < h; y++)
          for (x = 0; x < w; x++)
            {
              srcPix = src.getDataElements (x + srcRect.x, y + srcRect.y, srcPix);
              dstPix = dstIn.getDataElements (x + dstInRect.x, y + dstInRect.y, dstPix);
              int sp = srcColorModel.getRGB (srcPix);
              int dp = dstColorModel.getRGB (dstPix);
              int rp = sp ^ xp ^ dp;
              dstOut.setDataElements (x + dstOutRect.x, y + dstOutRect.y, 
                                      dstColorModel.getDataElements (rp, null));
            }
      }
    }
    
    public BitwiseXorComposite (Color c)
    {
      xorColor = c;
    }
    
    public CompositeContext createContext (ColorModel srcColorModel, 
                                           ColorModel dstColorModel, 
                                           RenderingHints hints) 
    {
      return new BitwiseXorCompositeContext (srcColorModel, dstColorModel);
    }
  }  


  ///////////////////////////////////////////////
  ////// Unimplemented Stubs and Overloads //////
  ///////////////////////////////////////////////

  public boolean drawImage(Image image, 
                           AffineTransform xform,
                           ImageObserver obs)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
  
  public void drawImage(BufferedImage image,
                        BufferedImageOp op,
                        int x,
                        int y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
  
  public void drawRenderedImage(RenderedImage image,
                                AffineTransform xform)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
  
  public void drawRenderableImage(RenderableImage image,
                                  AffineTransform xform)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawString(String text, float x, float y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
  
  public void drawString(AttributedCharacterIterator iterator,
                         float x, float y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
    
  public boolean hit(Rectangle rect, Shape text,
                     boolean onStroke)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public GraphicsConfiguration getDeviceConfiguration()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void setComposite(Composite comp)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void setRenderingHint(RenderingHints.Key hintKey,
                               Object hintValue)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public Object getRenderingHint(RenderingHints.Key hintKey)
  {
    throw new java.lang.UnsupportedOperationException ();
  }
  
  public void setRenderingHints(Map hints)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void addRenderingHints(Map hints)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public RenderingHints getRenderingHints()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void shear(double shearX, double shearY)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public Composite getComposite()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public FontRenderContext getFontRenderContext ()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawGlyphVector (GlyphVector g, float x, float y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void copyArea (int x, int y, int width, int height, int dx, int dy)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawArc (int x, int y, int width, int height,
                       int startAngle, int arcAngle)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public boolean drawImage (Image img, int x, int y, Color bgcolor, 
                            ImageObserver observer)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public boolean drawImage (Image img, int x, int y, int width, int height, 
                            Color bgcolor, ImageObserver observer)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public boolean drawImage (Image img, int x, int y, int width, int height, 
                            ImageObserver observer)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public boolean drawImage (Image img, int dx1, int dy1, int dx2, int dy2, 
                            int sx1, int sy1, int sx2, int sy2, 
                            Color bgcolor, ImageObserver observer)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public boolean drawImage (Image img, int dx1, int dy1, int dx2, int dy2, 
                            int sx1, int sy1, int sx2, int sy2, 
                            ImageObserver observer) 
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawOval(int x, int y, int width, int height)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawRoundRect(int x, int y, int width, int height, 
                            int arcWidth, int arcHeight)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawString (String str, int x, int y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void drawString (AttributedCharacterIterator ci, int x, int y)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void fillArc (int x, int y, int width, int height, 
                       int startAngle, int arcAngle)
  {
    cairoNewPath ();
    walkPath (new Arc2D.Double((double)x, (double)y, 
                               (double)width, (double)height,
                               (double)startAngle, (double)arcAngle,
                               Arc2D.PIE).getPathIterator (null));
    cairoClosePath ();
    cairoFill ();
  }

  public void fillOval(int x, int y, int width, int height)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void fillRoundRect (int x, int y, int width, int height, 
                             int arcWidth, int arcHeight)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public Font getFont ()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public FontMetrics getFontMetrics ()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public FontMetrics getFontMetrics (Font f)
  {
    throw new java.lang.UnsupportedOperationException ();
  }

  public void setFont (Font f)
  {
    if (f instanceof GdkFont)
      font = (GdkFont) f;
    else
      font = new GdkFont (f.getAttributes ());
  }

  public String toString()
  {
    throw new java.lang.UnsupportedOperationException ();
  }

}
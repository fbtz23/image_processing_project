// projekt1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _USE_MATH_DEFINES
#include<iostream>
#include <stack>
#include <math.h>
#include <vector>

struct punkt
{
  int x;
  int y;
};

using namespace std;
using namespace System;
using namespace System::Drawing;
//wyrownanie histogramu
void histogramRGB( Bitmap^ obraz, double odch, std::string nazwa_obrazu );
void histogramMono( Bitmap^ obraz, double odch, std::string nazwa_obrazu );

//ordfilt
void ordFilt2RGB( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu );
void ordFilt2Mono( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu );

//otwarcie elementem linjnym
void open( Bitmap^& obraz, int length, int dgr, std::string nazwa_obrazu );
//wypelnianie dziur
void holeFillLogic( Bitmap^ obraz, std::string nazwa_obrazu );

//rysowanie linii pod zadanym k¹tem
void BresenhamLine( const int x1, const int y1, const int x2, const int y2, vector< vector<bool> >& SE );
//void CenterLine( vector<punkt>& SE );
void erosion( Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width );
void dilation( Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width );

void menu( int choice, Bitmap^& obraz, std::string nazwa_obrazu );
int main()
{
    std::cout << "Podaj nazwe obrazu!\n";
    std::string nazwa_obrazu;
    cin >> nazwa_obrazu;
    Bitmap^ obraz;
    String^ newSystemString = gcnew String( nazwa_obrazu.c_str() );
    obraz = gcnew Bitmap( newSystemString, true );

    int choice;
    cout << "Co wykonac, 1) Wypelnianie dziur \n2)Otwarcie elementem linijnym" << endl;
    cin >> choice;
    menu( choice, obraz, nazwa_obrazu );
}

void menu( int choice, Bitmap^& obraz, std::string nazwa_obrazu )
{
    switch ( choice )
    {
    case 1:      
      cout << "Wybrano wypelnianie dziur\n";
      holeFillLogic( obraz, nazwa_obrazu );

      break;

    case 2:   
      cout << "Wybrano otwarcie elementem linijnym.\n";
      cout << "Podaj dlugosc odcinka\n";
      int dlug;
      cin >> dlug;
      cout << "Podaj kat w stopniach\n";
      int kat;
      cin >> kat;
      open( obraz, dlug, kat, nazwa_obrazu );
      break;

    case 3:   
      cout << "Ordfilt2";
      break;

    default:  
      cout << "Zla opcja";
    }
}
void open( Bitmap^& obraz, int length, int dgr, std::string nazwa_obrazu )
{
  dgr = dgr % 360;
  double radian = dgr * double( M_PI ) / 180;
  int ptk_1 = (cos( radian ) * length);
  int ptk_2 = (sin( radian ) * length);

  int SE_width = static_cast< int >( ptk_1 ); 
  int SE_height = static_cast< int >( ptk_2 );
  if ( SE_width % 2 == 0 ) ++SE_width;
  if ( SE_height % 2 == 0 ) ++SE_height;
  int centralPointX = ( SE_width - 1 ) / 2;
  int centralPointY = ( SE_height - 1 ) / 2;
  vector<vector<bool>> vect;
  vect.resize( SE_height );
  for ( int i = 0; i < SE_height; ++i )
    vect[ i ].resize( SE_width );
  for ( int i = 0; i < SE_height; i++ )
  {
    for ( int j = 0; j < SE_width; j++ )
    {
      cout << vect[ i ][ j ] << "\t";

    }
    cout << "\n";
  }
  int x0 = SE_height - 1;
  int y0 = 0;
  int x1 = 0;
  int y1 = ( SE_width - 1 );
  BresenhamLine( x0, y0, x1, y1, vect );

  //vect[ x0 ][ y0 ] = 1;
  //vect[ x1 ][ y1 ] = 1;


  //int dx = abs( x1 - x0 );
  //int sx = x0 < x1 ? 1 : -1;
  //int dy = -abs( y1 - y0 );
  //int sy = y0 < y1 ? 1 : -1;
  //int err = dx + dy;
  //while ( true ) {

  //  if ( x0 >= x1 && y0 >= y1 ) break;
  //  vect[ x0 ][ y0 ] = 1;

  //  int e2 = 2 * err;
  //  if ( e2 >= dy ) {
  //    err += dy;
  //    x0 += sx;
  //  }
  //  if ( e2 <= dx ) {

  //    err += dx;
  //    y0 += sy;
  //  }

  //}
  cout << " \n\n LINIA \n\n";
  for ( int i = 0; i < SE_height; i++ )
  {
    for ( int j = 0; j < SE_width; j++ )
    {
      cout << vect[ i ][ j ] << "\t";

    }
    cout << "\n";
  }
  Bitmap^ output  = gcnew Bitmap( obraz->Width, obraz->Height );
  Bitmap^ eroded = gcnew Bitmap( obraz->Width, obraz->Height );
  Bitmap^ dilated = gcnew Bitmap( obraz->Width, obraz->Height );
  dilation( dilated, obraz, vect, SE_height, SE_width );
  dilated->Save( "dilation.png" );
  erosion( eroded, obraz, vect, SE_height, SE_width );
  eroded->Save( "erosion.png" );
  dilation(output, eroded, vect, SE_height, SE_width );
  output->Save( "open.png" );
}
//void CenterLine( vector<punkt>& SE )
//{
//  int srodek = SE.size() / 2;
//  cout << srodek << " " << SE.size() << endl;
//  vector<punkt> temp = SE;
//  temp[ srodek ] = { 0,0 };
//  for ( int i = srodek; i > 0; i-- )
//  {
//    punkt dyst;
//    dyst.x = SE[ i - 1 ].x;
//    dyst.y = SE[ i - 1 ].y;
//    temp[ i - 1 ].x = SE[ i - 1 ].x - SE[ srodek ].x;
//    temp[ i - 1 ].y = SE[ i - 1 ].y - SE[ srodek ].y;
//  }
//  for ( int i = srodek; i < SE.size()-1; i++ )
//  {
//    punkt dyst;
//    dyst.x = SE[ i + 1 ].x;
//    dyst.y = SE[ i + 1 ].y;
//    temp[ i + 1 ].x = SE[ i + 1 ].x - SE[ srodek ].x;
//    temp[ i + 1 ].y = SE[ i + 1 ].y - SE[ srodek ].y;
//  }
//
//  SE = temp;
//}
void erosion(Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width )
{
  int wys = obraz->Height;
  int szer = obraz->Width;
  for ( int i = 0; i < szer; i++ )
  {
    for ( int j = 0; j < wys; j++ )
    {
      Color Px = obraz->GetPixel( j, i );
      int min{ Px.R };
      for ( int kx=0; kx<SE_height; kx++  )
      {
        for ( int kz = 0; kz < SE_width; kz++ )
        {
          if(SE[kx][kz])
            if ( ( j + kz >= 0 && j +kz < szer ) && ( i + kx >= 0 && i + kx < wys ) )
            {
              Color Px = obraz->GetPixel( j + kz, i + kx );
              if ( Px.R < min )
                min = Px.R;
            }
        }
      }
      output->SetPixel( j, i, Color::FromArgb( min, min, min ) );
    }
  }
}
void dilation( Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width )
{
  int wys = obraz->Height;
  int szer = obraz->Width;
  for ( int i = 0; i < szer; i++ )
  {
    for ( int j = 0; j < wys; j++ )
    {
      Color Px = obraz->GetPixel( j, i );
      int max{ Px.R };
      for ( int kx = 0; kx < SE_height; kx++ )
      {
        for ( int kz = 0; kz < SE_width; kz++ )
        {
          if ( SE[ kx ][ kz ] )
            if ( ( j + kx >= 0 && j + kx < szer ) && ( i + kz >= 0 && i + kz < wys ) )
            {
              Color Px = obraz->GetPixel( j + kx, i + kz );
              if ( Px.R > max )
                max = Px.R;
            }
        }
      }
      output->SetPixel( j, i, Color::FromArgb( max, max, max ) );
    }
  }
}
void BresenhamLine( const int x1, const int y1, const int x2, const int y2, vector< vector<bool> >& SE )
{
  // zmienne pomocnicze
  int d, dx, dy, ai, bi, xi, yi;
  int x = x1, y = y1;
  // ustalenie kierunku rysowania
  if ( x1 < x2 )
  {
    xi = 1;
    dx = x2 - x1;
  }
  else
  {
    xi = -1;
    dx = x1 - x2;
  }
  // ustalenie kierunku rysowania
  if ( y1 < y2 )
  {
    yi = 1;
    dy = y2 - y1;
  }
  else
  {
    yi = -1;
    dy = y1 - y2;
  }
  // pierwszy piksel
  SE[ x ][ y ] = true;
  //glVertex2i( x, y );
  // oœ wiod¹ca OX
  if ( dx > dy )
  {
    ai = ( dy - dx ) * 2;
    bi = dy * 2;
    d = bi - dx;
    // pêtla po kolejnych x
    while ( x != x2 )
    {
      // test wspó³czynnika
      if ( d >= 0 )
      {
        x += xi;
        y += yi;
        d += ai;
      }
      else
      {
        d += bi;
        x += xi;
      }
      SE[ x ][ y ] = true;
    }
  }
  // oœ wiod¹ca OY
  else
  {
    ai = ( dx - dy ) * 2;
    bi = dx * 2;
    d = bi - dy;
    // pêtla po kolejnych y
    while ( y != y2 )
    {
      // test wspó³czynnika
      if ( d >= 0 )
      {
        x += xi;
        y += yi;
        d += ai;
      }
      else
      {
        d += bi;
        y += yi;
      }
      SE[ x ][ y ] = true;
    }
  }
}
void histogramRGB( Bitmap^ obraz, double odch, std::string nazwa_obrazu ) {

}
void histogramMono( Bitmap^ obraz, double odch, std::string nazwa_obrazu )
{
}
void ordFilt2RGB( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu )
{
}
void ordFilt2Mono( Bitmap^ obraz, double mask, double num_porz, std::string nazwa_obrazu )
{
}
void holeFillLogic( Bitmap^ obraz, std::string nazwa_obrazu )
{
  Bitmap^ inverse;
  Bitmap^ output;
  Bitmap^ border;

  int wys, szer;
  int kz, kx;
  wys = obraz->Height;
  szer = obraz->Width;
  inverse = gcnew Bitmap( szer, wys );
  border = gcnew Bitmap( szer, wys );
  output = gcnew Bitmap( szer, wys );
  Color Px;
  for ( kz = 0; kz < wys; kz++ )
  {
    for ( kx = 0; kx < szer; kx++ )
    {
      Px = obraz->GetPixel( kx, kz );
      int pr, pg, pb;
      Px.R == 255 ? pr = 0 : pr = 255;
      Px.G == 255 ? pg = 0 : pg = 255;
      Px.B == 255 ? pb = 0 : pb = 255;
      inverse->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );

      border->SetPixel( 0, kz, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( szer-1, kz, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( kx, 0, Px.FromArgb( 255, 255, 255 ) );
      border->SetPixel( kx, wys-1, Px.FromArgb( 255, 255, 255 ) );

    }
  }
  while ( 1 )
  {
    for ( kz = 0; kz < wys; kz++ )
    {
      for ( kx = 0; kx < szer; kx++ )
      {
        output->SetPixel( kx, kz, Px.FromArgb( 0, 0, 0 ) );
        if ( inverse->GetPixel( kx, kz ).R )
        {
          if ( border->GetPixel( kx, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kx-1 >=0 && border->GetPixel( kx-1, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kx + 1 < szer && border->GetPixel( kx + 1, kz ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kz - 1 >= 0 && border->GetPixel( kx, kz -1 ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
          if ( kz + 1  < wys && border->GetPixel( kx, kz +1 ).R )
          {
            output->SetPixel( kx, kz, Px.FromArgb( 255, 255, 255 ) );
            continue;
          }
        }
      }
    }
    bool same = true;
    for ( kz = 0; kz < wys && same; kz++ )
    {
      for ( kx = 0; kx < szer && same; kx++ )
      {
        if ( border->GetPixel( kx, kz ).R != output->GetPixel( kx, kz ).R )
          same = false;
      }
    }
    if ( same )
      break;
    for ( kz = 0; kz < wys; kz++ )
    {
      for ( kx = 0; kx < szer; kx++ )
      {
        Px = output->GetPixel( kx, kz );
        int pr, pg, pb;
        pr = Px.R;
        pg = Px.G;
        pb = Px.B;
        border->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );
      }
    }
  }
  for ( kz = 0; kz < wys; kz++ )
  {
    for ( kx = 0; kx < szer; kx++ )
    {
      Px = output->GetPixel( kx, kz );
      int pr, pg, pb;
      Px.R == 255 ? pr = 0 : pr = 255;
      Px.G == 255 ? pg = 0 : pg = 255;
      Px.B == 255 ? pb = 0 : pb = 255;
      output->SetPixel( kx, kz, Px.FromArgb( pr, pg, pb ) );
    }
  }
  std:string nowa_nazwa = "filled_" + nazwa_obrazu;
  String^ newSystemString = gcnew String( nowa_nazwa.c_str() );
  output->Save( newSystemString );

}


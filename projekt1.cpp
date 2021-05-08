// projekt1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _USE_MATH_DEFINES
#include<iostream>
#include <stack>
#include <math.h>
#include <vector>
#include <algorithm>    // std::sort
struct punkt
{
  int x;
  int y;
};

using namespace std;
using namespace System;
using namespace System::Drawing;
//wyrownanie histogramu
void histogram( Bitmap^ obraz, double odch, std::string nazwa_obrazu );

//ordfilt
void ordFilt2( Bitmap^& obraz, int maskX, int maskY, int num_porz, std::string nazwa_obrazu );
bool checkIfMono( Bitmap^ obraz );
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
    cout << "Co wykonac, 1) Wypelnianie dziur \n2)Otwarcie elementem linijnym\n3)Ordfilt2\n4)Wyrownanie histogramu" << endl;
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
      cout << "Ordfilt2" << endl;
      cout << "podaj rozmiar maski, x" << endl;
      int mask_x;
      cin >> mask_x;
      cout << "podaj rozmiar maski, y" << endl;
      int mask_y;
      cin >> mask_y;
      cout << "podaj numer porzadkowy, <=x*y" << endl;
      int num_porz;
      cin >> num_porz;
      ordFilt2( obraz, mask_x, mask_y, num_porz, nazwa_obrazu );
      break;
    case 4:
      cout << "Wyrownanie histogramu" << endl;
      cout << "podaj odchylenie" << endl;
      double std;
      cin >> std;
      histogram( obraz, std, nazwa_obrazu );
      break;
    default:  
      cout << "Zla opcja";
    }
}
void open( Bitmap^& obraz, int length, int dgr, std::string nazwa_obrazu )
{
  dgr = dgr % 180;
  double radian = dgr * double( M_PI ) / 180;
  double ptk_1 = (cos( radian ) * length);
  double ptk_2 = (sin( radian ) * length);
  ptk_1 = round( ptk_1 );
  ptk_2 = round( ptk_2 );
  cout << dgr << " " << radian << " " << "ptk_1 " << ptk_1 << "ptk 2 " << ptk_2 << endl;
  int SE_width = static_cast< int >( ptk_1 ); 
  int SE_height = static_cast< int >( ptk_2 );
  if ( SE_width % 2 == 0 ) ++SE_width;
  if ( SE_height % 2 == 0 ) ++SE_height;

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

void erosion(Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width )
{
  int wys = obraz->Height;
  int szer = obraz->Width;
  int centralPointX = ( SE_width - 1 ) / 2;
  int centralPointY = ( SE_height - 1 ) / 2;
  for ( int i = 0; i < szer; i++ )
  {
    for ( int j = 0; j < wys; j++ )
    {
      Color Px = obraz->GetPixel( i, j );
      int min{ Px.R };
      for ( int kx= -centralPointY; kx<= centralPointY; kx++  )
      {
        for ( int kz = -centralPointX; kz <= centralPointX; kz++ )
        {
          if(SE[kx+ centralPointY][kz + centralPointX])
            if ( ( j + kx >= 0 && j +kx < wys ) && ( i + kz >= 0 && i + kz < szer ) )
            {
              Color Px = obraz->GetPixel( i + kz, j + kx );
              if ( Px.R < min )
                min = Px.R;
            }
        }
      }
      output->SetPixel( i, j, Color::FromArgb( min, min, min ) );
    }
  }
}
void dilation( Bitmap^& output, Bitmap^& obraz, vector< vector<bool> >& SE, int SE_height, int SE_width )
{
  int wys = obraz->Height;
  int szer = obraz->Width;
  int centralPointX = ( SE_width - 1 ) / 2;
  int centralPointY = ( SE_height - 1 ) / 2;
  for ( int i = 0; i < szer; i++ )
  {
    for ( int j = 0; j < wys; j++ )
    {
      Color Px = obraz->GetPixel( i, j );
      int max{ Px.R };
      for ( int kx = -centralPointY; kx <= centralPointY; kx++ )
      {
        for ( int kz = -centralPointX; kz <= centralPointX; kz++ )
        {
          if ( SE[ kx + centralPointY ][ kz + centralPointX ] )
            if ( ( j + kx >= 0 && j + kx < wys ) && ( i + kz >= 0 && i + kz < szer ) )
            {
              Color Px = obraz->GetPixel( i + kz, j + kx );
              if ( Px.R > max )
                max = Px.R;
            }
        }
      }
      output->SetPixel( i, j, Color::FromArgb( max, max, max ) );
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
void histogram( Bitmap^ obraz, double odch, std::string nazwa_obrazu )
{
  checkIfMono( obraz ) ? cout << "monochrom\n" : cout << "rgb\n";

  Bitmap^ output = gcnew Bitmap( obraz->Width, obraz->Height );
  Color Px;
  int N = 256;
  int wys, szer;
  int kz, kx;
  wys = obraz->Height;
  szer = obraz->Width;

  int** gray = new int* [ wys ];
  std::vector<int> histGr( N );

  int** blue = new int* [ wys ];
  int** green = new int* [ wys ];
  int** red = new int* [ wys ];

  std::vector<int> histB(N);
  std::vector<int> histG( N );
  std::vector<int> histR( N );

  std::vector<double> dyst(N);
  std::vector<int> LUT(N);

  std::vector<int> LUT_R( N );
  std::vector<int> LUT_G( N );
  std::vector<int> LUT_B( N );

  if ( !checkIfMono )
  {
    Color Px;

    for ( kz = 0; kz < wys; kz++ )
    {
      blue[ kz ] = new int[ szer ];
      green[ kz ] = new int[ szer ];
      red[ kz ] = new int[ szer ];
      for ( kx = 0; kx < szer; kx++ )
      {
        Px = obraz->GetPixel( kx, kz );
        //r
        red[ kz ][ kx ] = Px.R;
        histR[ red[ kz ][ kx ] ]++;
        //g
        green[ kz ][ kx ] = Px.G;
        histG[ green[ kz ][ kx ] ]++;
        //b
        blue[ kz ][ kx ] = Px.B;
        histB[ blue[ kz ][ kx ] ]++;
      }
    }
    for ( int i = 1; i < N; ++i )
    {
      histR[ i ] += histR[ i - 1 ];
      histG[ i ] += histG[ i - 1 ];
      histB[ i ] += histB[ i - 1 ];
    }
    int srednia = 127;
    for ( int i = 0; i < N; i++ )
      dyst[i]= exp( -1 * pow( i - srednia, 2 ) / ( 2. * pow( odch, 2 ) ) ) / ( odch * sqrt( 2. * M_PI ) );

    for ( int i = 1; i < N; i++ )
      dyst[ i ] += dyst[ i - 1 ];
    
    double multiplier = 1 / dyst[ 255 ];

    for ( int i = 0; i < 256; i++ ) {
      dyst[ i ] = dyst[ i ] * multiplier * wys * szer;
    }
    for ( int k = 0; k < 256; k++ ) {
      for ( int j = 0; j < 256; j++ ) {
        if ( histR[ k ] > dyst[ j ] ) {
          continue;
        }
        LUT_R[ k ] = j;
        break;
      }
    }
    for ( int k = 0; k < 256; k++ ) {
      for ( int j = 0; j < 256; j++ ) {
        if ( histG[ k ] > dyst[ j ] ) {
          continue;
        }
        LUT_G[ k ] = j;
        break;
      }
    }
    for ( int k = 0; k < 256; k++ ) {
      for ( int j = 0; j < 256; j++ ) {
        if ( histB[ k ] > dyst[ j ] ) {
          continue;
        }
        LUT_B[ k ] = j;
        break;
      }
    }
    for ( kz = 0;kz< wys; kz++ ) {
      for ( kx = 0; kx < szer; kx++ ) {
        red[ kz][ kx ] = LUT_R[ red[ kz ][ kx ] ];
        green[ kz ][ kx ] = LUT_G[ green[ kz ][ kx ] ];
        blue[ kz ][ kx ] = LUT_B[ blue[ kz ][ kx ] ];
        Px = Color::FromArgb( red[ kz ][ kx ], green[ kz ][ kx ], blue[ kz ][ kx ] );
        output->SetPixel( kx, kz, Px );
      }
    }
  }
  else
  {
    Color Px;
    for ( kz = 0; kz < 256; kz++ )
      histGr[ kz ] = 0;

    for ( kz = 0; kz < wys; kz++ )
    {
      gray[ kz ] = new int[ szer ];
      for ( kx = 0; kx < szer; kx++ )
      {
        Px = obraz->GetPixel( kx, kz );
        gray[ kz ][ kx ] = Px.R;
        histGr[ gray[ kz ][ kx ] ]++;
      }
    }
    for ( int i = 1; i < N; ++i )
      histGr[ i ] += histGr[ i - 1 ];

    int srednia = 127;
    for ( int i = 0; i < N; i++ )
      dyst[ i ] = exp( -1 * pow( i - srednia, 2 ) / ( 2. * pow( odch, 2 ) ) ) / ( odch * sqrt( 2. * M_PI ) );

    for ( int i = 1; i < N; i++ )
      dyst[ i ] += dyst[ i - 1 ];

    double multiplier = 1 / dyst[ 255 ];

    for ( int i = 0; i < 256; i++ ) {
      dyst[ i ] = dyst[ i ] * multiplier * wys * szer;
    }
    for ( int k = 0; k < 256; k++ ) {
      for ( int j = 0; j < 256; j++ ) {
        if ( histGr[ k ] > dyst[ j ] ) {
          continue;
        }
        LUT[ k ] = j;
        break;
      }
    }
    for ( kz = 0; kz < wys; kz++ ) {
      for ( kx = 0; kx < szer; kx++ ) {
        gray[ kz ][ kx ] = LUT[ gray[ kz ][ kx ] ];
        Px = Color::FromArgb( gray[ kz ][ kx ], gray[ kz ][ kx ], gray[ kz ][ kx ] );
        output->SetPixel( kx, kz, Px );
      }
    }
  }
  output->Save( "histogram.png" );
  for ( int i = 0; i < wys; ++i )
    delete[] gray[ i ];
  delete gray;
}
void ordFilt2( Bitmap^& obraz, int maskX, int maskY, int num_porz, std::string nazwa_obrazu )
{
  checkIfMono( obraz ) ? cout << "monochrom\n" : cout << "rgb\n";
  vector<vector<bool>> vect;
  vect.resize( maskY );
  for ( int i = 0; i < maskY; ++i )
    vect[ i ].resize( maskX );
  for ( int i = 0; i < maskY; i++ )
  {
    for ( int j = 0; j < maskX; j++ )
    {
      cout << vect[ i ][ j ] << "\t";

    }
    cout << "\n";
  }
  Bitmap^ output = gcnew Bitmap( obraz->Width, obraz->Height );
  int centralPointX = ( maskX - 1 ) / 2;
  int centralPointY = ( maskY - 1 ) / 2;
  if ( checkIfMono )
  {
    int wys = obraz->Height;
    int szer = obraz->Width;
    for ( int i = 0; i < szer; i++ )
    {
      for ( int j = 0; j < wys; j++ )
      {
        Color Px = obraz->GetPixel( i, j );
        int res = 0;
        vector<int> wyniki;
        for ( int kx = -centralPointY; kx <= centralPointY; kx++ )
        {
          for ( int kz = -centralPointX; kz <= centralPointX; kz++ )
          {
              if ( ( j + kx >= 0 && j + kx < wys ) && ( i + kz >= 0 && i + kz < szer ) )
              {
                Color Px = obraz->GetPixel( i + kz, j + kx );
                wyniki.push_back( Px.R );
              }
          }
        }
        std::sort( wyniki.begin(), wyniki.end() );
        if(wyniki.size() == num_porz )
          output->SetPixel( i, j, Color::FromArgb( wyniki[num_porz-1], wyniki[ num_porz - 1 ], wyniki[ num_porz - 1 ] ) );
        else
        {
          int pom_num = num_porz - wyniki.size();
          output->SetPixel( i, j, Color::FromArgb( wyniki[ num_porz - 1 - pom_num ], wyniki[ num_porz - 1 - pom_num ], wyniki[ num_porz - 1 - pom_num ] ) );
        }

      }
    }
  }
  else
  {
    int wys = obraz->Height;
    int szer = obraz->Width;
    for ( int i = 0; i < szer; i++ )
    {
      for ( int j = 0; j < wys; j++ )
      {
        Color Px = obraz->GetPixel( i, j );
        int res = 0;
        vector<int> wynikiR;
        vector<int> wynikiG;
        vector<int> wynikiB;
        for ( int kx = -centralPointY; kx <= centralPointY; kx++ )
        {
          for ( int kz = -centralPointX; kz <= centralPointX; kz++ )
          {
            if ( ( j + kx >= 0 && j + kx < wys ) && ( i + kz >= 0 && i + kz < szer ) )
            {
              Color Px = obraz->GetPixel( i + kz, j + kx );
              wynikiR.push_back( Px.R );
              wynikiG.push_back( Px.G );
              wynikiB.push_back( Px.B );

            }
          }
        }
        std::sort( wynikiR.begin(), wynikiR.end() );
        std::sort( wynikiG.begin(), wynikiG.end() );
        std::sort( wynikiB.begin(), wynikiB.end() );
        if ( wynikiR.size() == num_porz )
          output->SetPixel( i, j, Color::FromArgb( wynikiR[ num_porz - 1 ], wynikiG[ num_porz - 1 ], wynikiB[ num_porz - 1 ] ) );
        else
        {
          int pom_num = num_porz - wynikiR.size();
          output->SetPixel( i, j, Color::FromArgb( wynikiR[ num_porz - 1 - pom_num ], wynikiG[ num_porz - 1 - pom_num ], wynikiB[ num_porz - 1 - pom_num ] ) );
        }

      }
    }
  }
  output->Save( "ordfilt2.png" );
}
bool checkIfMono( Bitmap^ obraz )
{
  bool mono = true;
  int wys = obraz->Height;
  int szer = obraz->Width;
  for ( int i = 0; i < szer && mono; i++ )
  {
    for ( int j = 0; j < wys && mono; j++ )
    {
      Color Px = obraz->GetPixel( i, j );
      if ( !( Px.R == Px.G && Px.R == Px.B && Px.G == Px.B ) )
        mono = false;
    }
  }
  return mono;
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


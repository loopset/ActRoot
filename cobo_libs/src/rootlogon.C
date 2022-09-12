// From Chez Moi
 
{
   gROOT->SetStyle("Plain");// Default white background for all plots
     
   gStyle->SetCanvasColor(10);
   gStyle->SetStatColor(10);
   gStyle->SetPadColor(10);
   
   // Settings for statistics information
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(1001111);
   
   // SetPaperSize wants width & height in cm: A4 is 20,26 & US is 20,24
   gStyle->SetPaperSize(20,26); 
   int font = 42; 
   gStyle->SetDrawBorder(0);
   gStyle->SetTitleFont(font);
   gStyle->SetStatFont(font);
   gStyle->SetTextFont(font);
   gStyle->SetTitleFontSize(0.05);
   gStyle->SetTextSize(0.05);
   gStyle->SetCanvasBorderMode(0);
   gStyle->SetPadBorderMode(0);
   gStyle->SetTitleSize(0.06,"xyz");
   gStyle->SetLabelSize(0.035,"xyz");
   gStyle->SetLabelOffset(0.01,"xyz");
   gStyle->SetTitleOffset(1.0,"xyz");
   gStyle->SetLabelFont(12,"xyz");
   gStyle->SetTitleFont(font,"xyz");
   gStyle->SetMarkerSize(1.2);
   gStyle->SetOptTitle(1); 
   gStyle->SetEndErrorSize(5);
   gStyle->SetPadLeftMargin(0.15);  
   gStyle->SetPadBottomMargin(0.15);
   
   // Palette Building
   TColor::InitializeColors();

    const Int_t NRGBs = 5;
    const Int_t NCont = 99;
    Double_t stops[NRGBs] = { 0.00, 0.25, 0.5, 0.75, 1.00 };
    
//     Double_t red[NRGBs]   = { 0.00, 0.40, 0.60, 0.80, 1.00 };
//     Double_t green[NRGBs] = { 0.00, 0.40, 0.60, 0.80, 1.00 };
//     Double_t blue[NRGBs]  = { 0.00, 0.40, 0.60, 0.80, 1.00 };
    
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

	
	gStyle->SetPalette(1,0);         // 1->violet to red   >50->deep sea colors  
}

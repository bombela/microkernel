unit ScrolBox;

{
  Bombela
  20/06/2004
  20/06/2004
  
  Petite ScrollBox limitée à 255 strings.
  Le pointer DATA de la TStringList (Strings)
  est la couleur du texte.
  
}


interface

uses CRT, TextList;

type TScrollBox = object

  private
    FCursorX, FCursorY: byte;
    TopLine: byte;
    MargeBotom, MargeWidth: byte;
    LeftLine, LargeLine: byte;
    SauvCursorVisible, CursorVisible: boolean;
    procedure SaveContext;
    procedure RestoreContext;
    procedure PaintLigne(Index: byte; Num: byte);
    procedure PaintLigneSelect(Selected: byte);

  public
    Top, Left, Heigth, Width: byte;
    BorderColor: byte;
    ScrollColor: byte;
    ScrollButtonColor: byte;
    SelectedColor: byte;
    Strings: TStringList;
    constructor Init;
    destructor Free;
    procedure Paint(Delete: boolean);
    procedure SetCursorBotom;
    function Select(var Selected: byte): boolean;
     
  end;

implementation

constructor TscrollBox.Init;
begin
  Top := 10;
  Left := 10;
  Heigth := 10;
  Width := 50;
  BorderColor := white;
  TopLine := 0;
  MargeBotom := 0;
  MargeWidth := 0;
  ScrollColor := 13;
  ScrollButtonColor := 14;
  SelectedColor := 7;
  LeftLine := 0;
  LargeLine := 0;
  CursorVisible := true;
  Strings.Init;
end;

destructor TscrollBox.Free;
begin
  Strings.Free;
end;

procedure TscrollBox.SaveContext;
begin
  FCursorX := WhereX;
  FCursorY := WhereY;
end;

procedure TscrollBox.RestoreContext;
begin
  GotoXY(FCursorX, FCursorY);
  NormVideo;
end;

procedure TscrollBox.Paint(Delete: boolean);
var b,b2, b3: byte;
    Text: string;
begin
  SaveContext;
    
  if Delete then TextColor(Black) else TextColor(BorderColor);
  
  GotoXY(Left, Top);
  write('Ú');
  for b := 1 to Width-2 do write('Ä');
  writeln('¿');
  
  for b := 1 to Heigth-2 do
  begin
    GotoXY(Left, WhereY);
    write('³');
    for b2 := 1 to Width-2 do write(' ');
    writeln('³');
  end;
  
  GotoXY(Left, WhereY);
  write('À');  
  for b := 1 to Width-2 do write('Ä');
  writeln('Ù');

  If (Strings.count > 0) and (not Delete) then
  begin
 
    if TopLine >= Strings.count then TopLine := 0;

    b3 := (TopLine+Heigth-2-MargeBotom);
    if b3 > Strings.count then b3 := Strings.count;

    if Strings.count > Heigth-2 then MargeWidth := 1 else MargeWidth := 0;
 
    LargeLine := 0;
    for b := TopLine to b3-1 do
    begin
      Strings.GetText(b,Text);
      if length(Text) > LargeLine then LargeLine := length(Text);
    end;

    if LeftLine+Width-2-MargeWidth > LargeLine then LeftLine := LargeLine-Width+2+MargeWidth; 

    if LargeLine > Width-2-MargeWidth then
    begin
      textcolor(ScrollColor);
      GotoXY(Left+1,Top+Heigth-2);
      for b := 1 to Width-2-MargeWidth do write('°');
      GotoXY(Left+1+trunc((LeftLine / ((LargeLine-(Width-2-MargeWidth)) / (Width-3-MargeWidth)))),Top+Heigth-2);
      textcolor(ScrollButtonColor);
      write('Û');      
      MargeBotom := 1;
    end else
    begin
      MargeBotom := 0;
      LargeLine := 0;
      LeftLine := 0;
    end;
    
    b3 := (TopLine+Heigth-2-MargeBotom);
    if b3 > Strings.count then b3 := Strings.count;

    If Strings.count > Heigth-2 then
    begin
      textcolor(ScrollColor);
      for b := Top+1 to Top+Heigth-2-MargeBotom do
      begin
        GotoXY(Left+Width-2,b);
        write('°');
      end;
      GotoXY(Left+Width-2,Top+1+trunc((TopLine / ((Strings.count-(Heigth-2-MargeBotom)) / (Heigth-3-MargeBotom)) )));
      textcolor(ScrollButtonColor);
      write('Û');
    end;
  
    b2 := 0;
    for b := TopLine+1 to b3 do
    begin
      inc(b2);
      PaintLigne(b-1, b2);
    end;
        
  end;

  RestoreContext;
end;

procedure TscrollBox.PaintLigne(Index: byte; Num: byte);
var Txt: string;
    Color, i,i2: byte;
    Selectable: boolean;
begin
  GotoXY(Left+1,Top+Num);
  Strings.Get(Index,Txt,Color,Selectable);
  TextColor(byte(Color));
  i2 := LeftLine+(Width-2)-MargeWidth;
  if i2 > length(Txt) then i2 := length(Txt);
  for i := LeftLine+1 to i2 do write(Txt[i]);
end;

procedure TscrollBox.PaintLigneSelect(Selected: byte);
var Txt: string;
    Color: pointer;
begin
  if strings.count > 0 then
  begin
    SaveContext;
    TextBackGround(SelectedColor);
    PaintLigne(Selected,Selected+1-TopLine);
    RestoreContext;
  end;
end;

function TscrollBox.Select(var Selected: byte): boolean;
const Key_Echap = #27;
      Key_Up = #72;
      Key_Down = #80;
      Key_Left = #75;
      Key_Width = #77;
      Key_Enter = #13;
var Key: char;
    Selectable: boolean;
begin
  {ShowCursor(false);}

  Select := false;
  Selected := 0;
  
  Paint(False);
  PaintLigneSelect(Selected);
  
  repeat
    Key := ReadKey;
    If Key = #0 then
    begin
      Key := ReadKey;
      case Key of
        Key_Up:	begin
									if Selected > 0 then
									begin
										dec(Selected);
										if Selected < TopLine then
										begin
											dec(TopLine);
										end;
										Paint(False);
										PaintLigneSelect(Selected);
									end;         	    
                end;
        Key_Down:	begin
        	   				if Selected < Strings.count-1 then
        	   				begin
                    	inc(Selected);
         	     				if Selected-TopLine > Heigth-3-MargeBotom then
		     							begin
         	      				inc(TopLine);
         	     				end;
         	      			Paint(False);
 		     						 PaintLigneSelect(Selected);
         	    			end;         	    
                 	end;
        Key_Left:	begin
										if LeftLine > 0 then
										begin
											dec(LeftLine);
											Paint(False);
											PaintLigneSelect(Selected);
										end;
                 	end;
        Key_Width:begin
										if LeftLine+Width-MargeWidth-2 < LargeLine then
										begin        
											inc(LeftLine);
											Paint(False);
											PaintLigneSelect(Selected); 
										end;
									end;
				end;
    end else
      if (key = Key_Enter) and (strings.count > 0) then
      begin
        Strings.GetSelectable(Selected,Selectable);
        if Selectable then
        begin
        	Key := Key_Echap;
        	Select := true;
        end;
      end;
  until Key = Key_Echap;
  
  {ShowCursor(true);}
end;

procedure TscrollBox.SetCursorBotom;
begin
  GotoXY(1, Top+Heigth);
end;

end.
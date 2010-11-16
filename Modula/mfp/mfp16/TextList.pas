unit TextList;

{
  Bombela
  20/06/2004
  20/06/2004
  
  Petite StringList limitée à 255 strings.
  Chaque item à une couleur et un statut de possiblilitée d'être selectionné.
}

interface

type TListItem = record
    Text: String;
    Color: byte;
    Selectable: boolean;
    Next: pointer;
  end;
type PListItem = ^TListItem;

type TStringList = object

  private
    FCount: byte;
    FirstItem: PListItem;
  
  public
    constructor Init;
    destructor Free;
    function Add(Text: string; Color: byte; Selectable: boolean): boolean;
    function Count: byte;
    function Del(Index: byte): boolean;
    function Ins(Index: byte; Text: string; Color: byte; Selectable: boolean): boolean;
    procedure Clear;
    function Get(Index: byte; var Text: string; var Color: byte; var Selectable: boolean): boolean;
    function GetText(Index: byte; var Text: string): boolean;
    function GetData(Index: byte; var Color: byte): boolean;
    function GetSelectable(Index: byte; var Selectable: boolean): boolean;
    function Change(Index: byte; Text: string; Color: byte; Selectable: boolean): boolean;
  end;

implementation

constructor TStringList.Init;
begin
  FCount := 0;
  FirstItem := nil;
end;

destructor TStringList.Free;
var i: byte;
begin
  Clear;
end;

function TStringList.Add(Text: string; Color: byte; Selectable: boolean): boolean;
var Item: PListItem;
begin
  if FCount < 255 then
  begin
    If FCount = 0 then
    begin
      new(Item);
      FirstItem := Item;
    end else
    begin
      Item := FirstItem;
      while (Item^.Next <> nil) do Item := Item^.Next;
      new(PListItem(Item^.Next));
      Item := Item^.Next;
    end;
    Item^.Next := nil;
    Item^.Text := Text;
    Item^.Color := Color;
    Item^.Selectable := Selectable;
    inc(FCount);
    Add := true;
  end else Add := false;    
end;

function TStringList.Get(Index: byte; var Text: string; var Color: byte; var Selectable: boolean): boolean;
var i: byte; Item: PListItem;
begin
  if (Index < FCount) then
  begin
    Item := FirstItem;
    if Index > 0 then 
    	for i := 0 to Index-1 do Item := Item^.Next;
    Text := Item^.Text;
    Color := Item^.Color;
    Selectable := Item^.Selectable;
    Get := true;
  end else Get := false;
end;

function TStringList.GetText(Index: byte; var Text: string): boolean;
var Color: byte;
    Selectable: boolean;
begin
  GetText := Get(Index,Text,Color,Selectable);
end;

function TStringList.GetData(Index: byte; var Color: byte): boolean;
var Text: string;
    Selectable: boolean;
begin
  GetData := Get(Index,Text,Color,Selectable);
end;

function TStringList.GetSelectable(Index: byte; var Selectable: boolean): boolean;
var Text: string;
    Color: byte;
begin
  GetSelectable := Get(Index,Text,Color,Selectable);
end;

function TStringList.Count: byte;
begin
  Count := FCount;
end;

function TStringList.Del(Index: byte): boolean;
var i: byte;
    Item1, Item2: PListItem;
begin
  if Index < FCount then
  begin
    if Index = 0 then
    begin
      Item1 := FirstItem^.Next;
      Dispose(FirstItem);
      FirstItem := Item1;
    end else
    begin
      Item1 := FirstItem;
      if Index > 1 then
      		for i := 0 to Index-2 do Item1 := Item1^.Next;
      Item2 := PListItem(Item1^.Next)^.Next;
      Dispose(PListItem(Item1^.Next));
      Item1^.Next := Item2;
     end;
    dec(FCount);
    Del := true;
  end else Del := false;
end;

function TStringList.Ins(Index: byte; Text: string; Color: byte; Selectable: boolean): boolean;
var i: byte;
    Item1, Item2: PListItem;
begin
  if (Index < FCount) and (FCount < 255) then
  begin
    if Index = 0 then
    begin
      Item1 := FirstItem;
      new(FirstItem);
      FirstItem^.Next := Item1;
      Item1 := FirstItem;
    end else
    begin
      Item1 := FirstItem;
      if Index > 1 then
        if Index = 2 then Item1 := FirstItem^.Next
      		else for i := 0 to Index-2 do Item1 := Item1^.Next;
      Item2 := Item1^.Next;
      new(PListItem(Item1^.Next));
      Item1 := Item1^.Next;
      Item1^.Next := Item2;
    end;
    Item1^.Text := Text;
    Item1^.Color := Color;
    Item1^.Selectable := Selectable;
    inc(FCount);
    Ins := true;
  end else Ins := false;
end;

procedure TStringList.Clear;
var Item: PListItem;
begin  
  Item := FirstItem;
  while Item <> nil do
  begin
    Item := Item^.Next;
    Dispose(FirstItem);
    FirstItem := Item;
  end;
  FCount := 0;
end;

function TStringList.Change(Index: byte; Text: string; Color: byte; Selectable: boolean): boolean;
var i: byte;
    Item: PListItem;
begin
  if Index < FCount then
  begin
    Item := FirstItem;
    If Index > 0 then for i := 0 to index do Item := Item^.Next;
    Item^.Text := Text;
    Item^.Color := Color;
    Item^.Selectable := Selectable;
  end;
end;

end.
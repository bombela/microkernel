unit Img;
{$G+}
                        interface
uses crt;

type TQBVR = packed record
Blue: byte;
Green: byte;
Red: byte;
Reserved: byte;
end;

procedure VGA_SaveMode;
procedure VGA_RestoreMode;
procedure VESA_SetMode(Mode: word);
procedure VESA_PaintBMPfile(FileName: string;Offset: Longint;X,Y,ModeLarge: word);
function  VESA_Test: boolean;

                         implementation

var OldMode: byte;
    CurrentWindow: word;

procedure PutPixel(X,Y,Larg: word; Pixel: Tqbvr); external;
{$L PUTPIX.OBJ}

function IntToStr(I: Longint): String;
var
 S: string[11];
begin
 Str(I, S);
 IntToStr := S;
end;

procedure VESA_PaintBMPfile(FileName: string;Offset: Longint;X,Y,ModeLarge: word);
var F: file; Hauteur, Largeur: longint; octet: byte;
    StartPixel, ix,iy: word; Pixel: tqbvr;
begin
assign(F,FileName);
{$I-}
reset(F,1);
{$I+}
if ioresult = 0 then begin
        seek(F,Offset);
        blockread(F,Hauteur,2);
        seek(F,Offset+10);
        blockread(F,StartPixel,4);
        seek(F,Offset+18);
        blockread(F,Largeur,4);
        blockread(F,Hauteur,4);

        seek(F,Offset+StartPixel);

            for iy := Y to Hauteur-1+Y do
            for ix := X to Largeur-1+X do
                begin
                  blockread(F,Pixel,3);
                  PutPixel(ix,iy,ModeLarge,pixel);
                end;
        close(f);
  end else begin sound(440); delay(500); nosound; end;
end;

procedure VESA_SetMode(Mode: word); assembler;
asm
   pusha
   mov ax, 4f02h
   mov bx, [Mode]
   int 10h
   popa
end;

procedure VGA_SaveMode; assembler;
asm
   pusha
   mov ah, 0fh
   int 10h
   mov [OldMode], al
   popa
end;

procedure VGA_RestoreMode; assembler;
asm
   pusha
   mov ah, 0h
   mov al, [OldMode]
   int 10h
   popa
end;

function  VESA_Test: boolean;
var VesaBuf: pointer;
begin
  VESA_Test := false;
  GetMem(VesaBuf,256);
  asm
    push di
    push es
    push ax
    
    mov ax, $4F00
    les di, [VesaBuf]
    int $10
    
    cmp ax, $004F
    jne @NoVesa
    cmp word ptr [es:di], $4556
    jne @NoVesa
    cmp word ptr [es:di+2], $4153
    jne @NoVesa

    mov [@Result], 1
    
    @NoVesa:
    
    pop ax
    pop es
    pop di
  end;
  FreeMem(VesaBuf,256);
end;

end.
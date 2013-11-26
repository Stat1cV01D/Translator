#include "AstUtils.h"
/*
TMemoryStream::~TMemoryStream()
{
	Clear();
}

void TMemoryStream::Clear()
{
	SetCapacity(0);
	FSize = 0;
	FPosition = 0;
}

void TMemoryStream::LoadFromStream(TMemoryStream &Stream)
{
	int Count;

	Stream.FPosition = 0;
	Count = Stream.GetSize();
	SetSize(Count);
	if (Count != 0) 
		Stream.ReadBuffer(FMemory, Count);
}

void TMemoryStream.SetCapacity(int NewCapacity)
{
	SetPointer(Realloc(NewCapacity), FSize);
	FCapacity := NewCapacity;
}

void TMemoryStream.SetSize(NewSize: Longint);
var
OldPosition: Longint;
begin
OldPosition := FPosition;
SetCapacity(NewSize);
FSize := NewSize;
if OldPosition > NewSize then Seek(0, soFromEnd);
end;

void *TMemoryStream.Realloc(var NewCapacity: Longint): Pointer;
begin
	if (NewCapacity > 0) and (NewCapacity <> FSize) then
NewCapacity := (NewCapacity + (MemoryDelta - 1)) and not (MemoryDelta - 1);
Result := Memory;
if NewCapacity <> FCapacity then
	begin
	if NewCapacity = 0 then
		begin
		FreeMem(Memory);
Result := nil;
end else
	begin
	if Capacity = 0 then
		GetMem(Result, NewCapacity)
	else
	ReallocMem(Result, NewCapacity);
if Result = nil then raise EStreamError.CreateRes(@SMemoryStreamError);
end;
end;
end;

int TMemoryStream.Write(const Buffer; Count: Longint);
var
Pos: Longint;
begin
	if (FPosition >= 0) and (Count >= 0) then
		begin
Pos := FPosition + Count;
if Pos > 0 then
	begin
	if Pos > FSize then
		begin
		if Pos > FCapacity then
			SetCapacity(Pos);
FSize := Pos;
end;
System.Move(Buffer, Pointer(Longint(FMemory) + FPosition)^, Count);
FPosition := Pos;
Result := Count;
Exit;
end;
end;
Result := 0;
end;
*/
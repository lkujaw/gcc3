-- { dg-do run }
-- { dg-options "-fstack-check" }

--  Test if the context is being properly adjusted.

with Text_IO;

procedure Stack_Check4 is
  I : Integer := 1;
begin
  loop
    declare
      Doubling_String : String (1 .. I) := (others => ' ');
    begin
      if Doubling_String (I) /= ' ' then
         raise Program_Error;
      end if;
    end;
    I := I * 2;
  end loop;
exception
   when Storage_Error => null;
end Stack_Check4;

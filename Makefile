AS = TASMX /m2
LN = TLINK
INCS = DATA.INC GLOBALS.INC SEGMENTS.INC ENUMS.INC DATA.INC

all: SUPAPLEX.EXE

clean:
	@ DEL BUILD\SUPAPLEX.OBJ
	@ DEL BUILD\UTILS.OBJ
	@ DEL BUILD\SUPAPLEX.LST
	@ DEL BUILD\SUPAPLEX.MAP
	@ DEL BUILD\SUPAPLEX.EXE

check:
	@ TEST.BAT

# can't get to work:
# .ASM.OBJ:
# 	DEL $@
# 	DEL $*.LST
# 	echo $** $@ $*.LST

SUPAPLEX.EXE: BUILD\SUPAPLEX.OBJ BUILD\UTILS.OBJ
	$(LN) $**

BUILD\SUPAPLEX.OBJ: SUPAPLEX.ASM $(INCS)
	$(AS) /m2 $&.ASM $@ $*.LST
BUILD\UTILS.OBJ: UTILS.ASM $(INCS)
	$(AS) /m2 $&.ASM $@ $*.LST

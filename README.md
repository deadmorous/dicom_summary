# DICOM summary

This program prints out a summary of a DICOM dataset, including image previews, to standard output.

## Dependencies
- [DICOMHERO (formerly Imebra)](https://dicomhero.com/)

## Usage

```{bash}
dicom_summary path_to_dir_containing_DICOMDIR_file
```

## Example output (cropped)

```
2_0: UL: 184 - File Meta Information Group Length
2_1: OB: {0, 1} - File Meta Information Version
2_2: UI: 1.2.840.10008.1.3.10 - Media Storage SOP Class UID
2_3: UI: 2.25.102871460008063533087638655264344817406 - Media Storage SOP Instance UID
2_10: UI: 1.2.840.10008.1.2.1 - Transfer Syntax UID
2_12: UI: 1.3.6.1.4.1.30071.8 - Implementation Class UID
2_13: SH: fo-dicom 4.0.8 - Implementation Version Name
2_16: AE: B3D - Source Application Entity Title
4_1130: CS: BIOTRONICS3D - File-set ID
4_1200: UL: 394 - Offset of the First Directory Record of the Root Directory Entity
4_1202: UL: 394 - Offset of the Last Directory Record of the Root Directory Entity
4_1212: US: 0 - File-set Consistency Flag
4_1220: SQ: [] - Directory Record Sequence
----
    4_1400: UL: 0 - Offset of the Next Directory Record
    4_1410: US: 65535 - Record In-use Flag
    4_1420: UL: 546 - Offset of Referenced Lower-Level Directory Entity
    4_1430: CS: PATIENT - Directory Record Type
    8_5: CS: ISO_IR 100 - Specific Character Set
    ...
----
...
                    43_10c5: LO: {;, ;} - UNKNOWN TAG
                    43_10c6: DS: {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, ...} - UNKNOWN TAG
                    43_10c7: DS: {0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, ...} - UNKNOWN TAG
                    43_10c8: UL: 0 - UNKNOWN TAG
                    43_10c9: IS: 24 - UNKNOWN TAG
                    7fe0_10: OB: [{}, {255, 79, 255, 81, 0, 41, 0, 0, 0, 0, ...}] - Pixel Data
                    Image 0
                    > size: 512 x 512
                    > color space: MONOCHROME2
                    > channels: 1
                    > depth: depthS16
                    > hi_bit: 15
                    >                                                             
                    >                           ..==,,                            
                    >                       __11ddSSDDaa))..                      
                    >               ``````>>kk@@QQWWmmWWOO[[__````                
                    >         ``````````''YYKKUU22hhYYSSbbWWFF``                  
                    > ``````  ````      ??PP99]]jjbb22ooaa22ww::            ``````
                    > ``````          ``FFkkaaZZSSEEPPxx55hhSS!!              ``  
                    >   ````          ``||kkwwYY00qqPPggooYYxx??              ``  
                    >   ````          ``vvSSaa22GGBBQQddEEooooss``            ``  
                    >   ````          ``TT55xxZZaahh44qqaakk||//              ``  
                    >   ``              <<((ooSSxxyySS55wwxxss<<                  
                    >   ``              ++((TTaann55qqZZaaCC33::                  
                    >   ``              ^^tt++ffYY5544]]55lluu``                  
                    >   ``              ``LLuu11hhOOPPwwttxx}}``                  
                    >   ``                ++ddJJFFeekk11??55))                    
                    >   ``                ::dd((zztt[[ffccaa<<                    
                    >                     ``ooaazz//??**rryy>>                    
                    >                 ``++llqqnnrr!!LL??rrtt33;;                  
                    >   ``    ..!!JJlloo55))!!//**ss))JJTT??((yy]]((::            
                    >   ``!!yy66FF!!zz77CCcc>>ccrrcc77))++//cc>>++77ttffii>>``    
                    > ``++{{11))??ccrrzz??**rrcc//cc||TTrr//cc>>rr((zz<<JJjj33^^  
                    > ^^;;****!!;;>>!!++cccc;;++((ss77ss))**;;;;++<<==,,^^>>ssvv==
                    > >>!!||JJFFss>>^^;;!!::````<<TTTTLLTT,,``..,,;;,,,,,,^^^^,,,,
                    > ==++**LL((!!==!!++--      ..!!****>>``      __==__^^>>==^^;;
                    > ;;rrss++rr==<<ss,,          ::<<++::          __^^__==^^__;;
                    > ++//cccc>>==zzcc..          ``^^<<::          ..^^::__^^^^++
                    > 77**;;^^rr++**,,              ,,==--``          __,,::__,,??
                    > ;;cc====++cc>>--              --__--``          ..,,__::>>^^
                    > __<<__^^;;<<,,``              ..::..``            ''__''<<--
                    > __''..--::,,''..              ..::--``            ``--..''__
                    Image count: 1
----
...
```

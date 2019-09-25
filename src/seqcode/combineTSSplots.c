/*************************************************************************
*                                                                        *
*   Module: combineTSSplots                                              *
*                                                                        *
*   CombineTSSplots main program                                         *
*                                                                        *
*   This file is part of the SeqCode 1.0 distribution                    *
*                                                                        *
*     Copyright (C) 2020 - Enrique BLANCO GARCIA                         *
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
*  This program is distributed in the hope that it will be useful,       *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*  GNU General Public License for more details.                          *
*                                                                        *
*  You should have received a copy of the GNU General Public License     *
*  along with this program; if not, write to the Free Software           * 
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
*************************************************************************/

#include "seqcode/seqcode.h"

/************************************************************************
                            ENVIRONMENTAL variables
************************************************************************/

/* Verbose flag (memory/processing information) */
int VRB=0;

/* Read extension */
int AVGL = 150;

/* Bin resolution */
int WINDOWRES = WINDOWRES_PLOTS;

/* Spike-in normalization */
int SPIKEIN = 0;

/* Background  color */
char BGCOLOR[MAXCOLORNAME];

/* Foreground color */
char FGCOLOR[MAXCOLORNAME];

/* Foreground color: color3 */
char FGCOLORB[MAXCOLORNAME];

/* General background color */
char GBGCOLOR[MAXCOLORNAME];

/* General foreground color */
char GFGCOLOR[MAXCOLORNAME];

/* Heatmap background color */
char HBGCOLOR[MAXCOLORNAME];

/* Uniform plot: 0-1 */
int UNIFORM = 0;

/* Colorize the heatmaps */
int COLORIZE = 1;

/* Pie chart colors */
char COLOR1[MAXCOLORNAME];
char COLOR2[MAXCOLORNAME];
char COLOR3[MAXCOLORNAME];
char COLOR4[MAXCOLORNAME];
char COLOR5[MAXCOLORNAME];
char COLOR6[MAXCOLORNAME];
char COLOR7[MAXCOLORNAME];

/* Pie chart gradient monocolor */
int GRADIENT = 0;

/* Piechart: count frequencies in three classes: promoters, genic and intergenic */
int SIMPLIFIED = FALSE;

/* Accounting time and results */
account *m;

/************************************************************************
                             MAIN program
************************************************************************/

int main (int argc, char *argv[])
{
  char ChromInfoFile[FILENAMELENGTH],
    RefgeneFile[FILENAMELENGTH],
    ReadsFile1[FILENAMELENGTH],
    ReadsFile2[FILENAMELENGTH],
    GenesFile[FILENAMELENGTH],
    TrackName1[FILENAMELENGTH],
    TrackName2[FILENAMELENGTH];

  int L;
  
  dict* ChrNames;
  long* ChrSizes;

  dict* GeneNames;
  int* GeneChrs;

  transcript** Transcripts;

  unsigned int** READS1;
  unsigned int** READS2;
  long* REGION;

  long FileSize;

  char OutputDirName[FILENAMELENGTH];
  char OutputFileName[FILENAMELENGTH];
  char RscriptFileName[FILENAMELENGTH];
  char PlotFileName[FILENAMELENGTH];

  dict* RColors;
  char RGBcodes[MAXCOLORS][MAXRGBCODE];

  int tmpSystem;
  
  char mess[MAXSTRING];

  
  /** 0. Starting and reading options, parameters and sequence... **/
  
  /* 0.a. Initializing stats and time counters */
  m = (account *) InitAcc();  
  
  /* 0.b. Read setup options */
  readargv_combineTSSplots(argc,argv,
			   ChromInfoFile,
			   RefgeneFile,
			   ReadsFile1,
			   ReadsFile2,
			   GenesFile,
			   TrackName1,
			   TrackName2,
			   &L);
  printSeqCodeHeader(COMBINETSSPLOTS);
  StartingTime(mess,m);
  printMess(mess);
  
  /* 1. Allocating data structures */
  printMess("1. Request Memory to Operating System");
  ChrNames = (dict *) RequestMemoryDictionary();
  ChrSizes = (long *) RequestMemoryChrSizes();
  GeneNames = (dict *) RequestMemoryDictionary();
  GeneChrs = (int *) RequestMemoryGeneChrs();
  Transcripts = (transcript**) RequestMemoryTranscripts();
  REGION = (long*) RequestMemoryRegion(L);
  RColors = (dict *) RequestMemoryDictionary();

  /* Color control */
  LoadRColors(RColors,RGBcodes);
  ValidateRColors(BGCOLOR,FGCOLOR,
		  GBGCOLOR,GFGCOLOR,
		  HBGCOLOR,FGCOLORB,
		  RColors);
  printRes("Loading the R color schema");

  /* 2. Read the ChrSize file */
  printMess("2. Reading Chromosome Sizes");
  m->nChrs = ReadChrFile (ChromInfoFile,ChrSizes,ChrNames);
  sprintf(mess,"Size was successfully acquired for %d chromosomes",m->nChrs);
  printRes(mess);

  /*  Additional allocating data structures */
  READS1 = (unsigned int**) RequestMemoryREADS(ChrSizes,ChrNames);
  READS2 = (unsigned int**) RequestMemoryREADS(ChrSizes,ChrNames);

  /* 3. Create the output files for the TSS representation*/
  printMess("3. Creating output files");
  
  sprintf(OutputDirName,"%s-%s_TSSplot_%d",TrackName1,TrackName2,L);
  sprintf(mess,"Creating the output folder %s",OutputDirName);
  printRes(mess);

  /* Creating the new Output folder */
  if (mkdir(OutputDirName,0777))
    {
      printRes("Removing the old output folder");
      sprintf(mess,"rm -rf %s",OutputDirName);
      printRes(mess);
      tmpSystem = system(mess);
      
      if (mkdir(OutputDirName,0777))
	{
	  printError("Problems with the creation of the output folder");
	}
    }

  /* Output files */
  sprintf(OutputFileName,"%s/TSSprofile_%s-%s_%d.txt",
	  OutputDirName,TrackName1,TrackName2,L);
  sprintf(mess,"Filename for the TSS average profile %s",OutputFileName);
  printRes(mess);

  sprintf(PlotFileName,"%s/PlotTSSprofile_%s-%s_%d.pdf",
	  OutputDirName,TrackName1,TrackName2,L);
  sprintf(mess,"PDF file for the plot file %s",PlotFileName);
  printRes(mess);

  /* 4. Read the Refgene file */
  printMess("4. Reading Refgene file");
  FileSize = GetFileSize(RefgeneFile);
  sprintf(mess,"%s: %.2lf Mb",RefgeneFile,(float)FileSize/MEGABYTE);
  printRes(mess);
  
  m->nTotalTranscripts = ReadRefgeneFile2(RefgeneFile,Transcripts,ChrNames,GeneNames,GeneChrs);
  sprintf(mess,"%ld transcripts of RefSeq were successfully acquired",m->nTotalTranscripts);
  printRes(mess);
  
  /* 5a. Acquire the BAM/SAM reads file 1 */
  printMess("5a. Processing the BAM/SAM file 1");
  
  FileSize = GetFileSize(ReadsFile1);
  sprintf(mess,"%s: %.2lf Mb",ReadsFile1,(float)FileSize/MEGABYTE);
  printRes(mess);

  ReadSAMFile(ReadsFile1,ChrSizes,ChrNames,READS1);    
  m->nReads1 = m->nReads;
  m->nFwdReads1 = m->nFwdReads;
  m->nRvsReads1 = m->nRvsReads;
  sprintf(mess,"%ld reads of the SAM file processed",m->nReads1);
  printRes(mess);                                               
  sprintf(mess,"%ld forward reads acquired",m->nFwdReads1);      
  printRes(mess);                                               
  sprintf(mess,"%ld reverse reads acquired",m->nRvsReads1);      
  printRes(mess);

  /* 5b. Acquire the BAM/SAM reads file 2 */
  printMess("5b. Processing the BAM/SAM file 2");
  
  FileSize = GetFileSize(ReadsFile2);
  sprintf(mess,"%s: %.2lf Mb",ReadsFile2,(float)FileSize/MEGABYTE);
  printRes(mess);

  ReadSAMFile(ReadsFile2,ChrSizes,ChrNames,READS2);    
  m->nReads2 = m->nReads;
  m->nFwdReads2 = m->nFwdReads;
  m->nRvsReads2 = m->nRvsReads;
  sprintf(mess,"%ld reads of the SAM file processed",m->nReads2);
  printRes(mess);                                               
  sprintf(mess,"%ld forward reads acquired",m->nFwdReads2);      
  printRes(mess);                                               
  sprintf(mess,"%ld reverse reads acquired",m->nRvsReads2);      
  printRes(mess);

  /* 6. Process the file of target genes */
  printMess("6. Processing the List of target genes");

  m->nGenes = CombineTargetGenes(GenesFile,Transcripts,
				 ChrNames,ChrSizes,
				 GeneNames,GeneChrs,
				 L,READS1,READS2,
				 REGION,
				 OutputFileName,
				 m->nReads1,
				 m->nReads2);
  sprintf(mess,"%d target genes were successfully processed",m->nGenes);
  printRes(mess);

  /* 7. Produce the final file to produce the TSS plot */
  if (m->nGenes>0)
    {
      printMess("7. Running R to produce the plots");

      sprintf(RscriptFileName,"%s/RscriptTSSprofile_%s-%s_%d.txt",
	      OutputDirName,TrackName1,TrackName2,L);
      sprintf(mess,"Preparing the Rscript file %s",RscriptFileName);
      printRes(mess);
      
      CreateRscriptCombined(OutputFileName,
			    RscriptFileName,
			    PlotFileName,
			    TrackName1,
			    TrackName2,
			    L,
			    m->nReads1,
			    m->nReads2,
			    m->nGenes);
      sprintf(mess,"R CMD BATCH %s",RscriptFileName);
      printRes(mess);
      tmpSystem = system(mess);
      
      printRes("Removing the out R file");
      sprintf(mess,"rm -f *.Rout");
      printRes(mess);
      tmpSystem = system(mess);
    }

  /* 7. The End */
  OutputTime(m);  
  
  exit(0);
  return(0);
}

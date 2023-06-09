/*
 *  main.c
 *  Projet 2022-2023
 *  Elasticite lineaire plane
 *
 *  Preprocesseur
 *
 *  Copyright (C) 2023 UCL-IMMC : Vincent Legat
 *  All rights reserved.
 *
 */
 
#include "glfem.h"

int main(void)
{  

//
//  -1- Construction de la geometrie 
//

    double Lx = 1.0;
    double Ly =  1.0;     
    geoInitialize();
    femGeo* theGeometry = geoGetGeometry();
    
    theGeometry->LxPlate     =  Lx;
    theGeometry->LyPlate     =  Ly;     
    theGeometry->h           =  Lx * 0.05;    
    theGeometry->elementType = FEM_TRIANGLE;
  
    geoMeshGenerate();      // Utilisation de OpenCascade
    
    // geoMeshGenerateGeo();   // Utilisation de outils de GMSH  
                            // Attention : les entit�s sont diff�rentes !
                            // On a aussi invers� la g�omtrie pour rire !
                            
//  geoMeshGenerateGeoFile("../data/mesh.geo");   // Lecture fichier geo
  
    geoMeshImport();
    // geoSetDomainName(0,"Contre poid gauche");
    // geoSetDomainName(1,"Contre poid haut");

    // geoSetDomainName(3,"Saut");
    // geoSetDomainName(7,"Contre poid bas");
    // geoSetDomainName(6,"Contre poid droit");
    geoMeshWrite("../data/mesh.txt");
          
//
//  -2- Definition du probleme
//
    
    double E   = 2.e9;
    double nu  = 0.37;
    double rho = 1.22e3; 
    double g   = 9.81;
    double m = -700 ; 
    femProblem* theProblem = femElasticityCreate(theGeometry,E,nu,rho,g,PLANAR_STRAIN);

    // femElasticityAddBoundaryCondition(theProblem,"Contre poid gauche",DIRICHLET_X,0.0);
    // femElasticityAddBoundaryCondition(theProblem,"Contre poid droit",DIRICHLET_X,0.0);
    // femElasticityAddBoundaryCondition(theProblem,"Saut",NEUMANN_N, 9.81 * m );
    // femElasticityAddBoundaryCondition(theProblem,"Contre poid bas",DIRICHLET_Y, 0 );
    // femElasticityAddBoundaryCondition(theProblem,"Contre poid haut",DIRICHLET_Y, 0 );
    femElasticityPrint(theProblem);
    femElasticityWrite(theProblem,"../data/problem.txt");
 

//
//  -3- Champ de la taille de r�f�rence du maillage
//

    double *meshSizeField = malloc(theGeometry->theNodes->nNodes*sizeof(double));
    femNodes *theNodes = theGeometry->theNodes;
    for(int i=0; i < theNodes->nNodes; ++i)
        meshSizeField[i] = theGeometry->geoSize(theNodes->X[i], theNodes->Y[i]);
    double hMin = femMin(meshSizeField,theNodes->nNodes);  
    double hMax = femMax(meshSizeField,theNodes->nNodes);  
    printf(" ==== Global requested h : %14.7e \n",theGeometry->h);
    printf(" ==== Minimum h          : %14.7e \n",hMin);
    printf(" ==== Maximum h          : %14.7e \n",hMax);
    
//
//  -4- Visualisation 
//  
    
    int mode = 1; 
    int domain = 0;
    int freezingButton = FALSE;
    double t, told = 0;
    char theMessage[MAXNAME];
    int n = theGeometry->theNodes->nNodes;
    for (int i=0; i<n; i++){
        if(theNodes->Y[i] >1.06 && theNodes->X[i] > 1.6)
        printf(" Y[i] , X[i]: %f, %f \n",theNodes->Y[i],theNodes->X[i]);
    }
 
    GLFWwindow* window = glfemInit("EPL1110 : Project 2022-23 ");
    glfwMakeContextCurrent(window);

    do {
        int w,h;
        glfwGetFramebufferSize(window,&w,&h);
        glfemReshapeWindows(theGeometry->theNodes,w,h);

        t = glfwGetTime();  
        if (glfwGetKey(window,'D') == GLFW_PRESS) { mode = 0;}
        if (glfwGetKey(window,'V') == GLFW_PRESS) { mode = 1;}
        if (glfwGetKey(window,'N') == GLFW_PRESS && freezingButton == FALSE) { domain++; freezingButton = TRUE; told = t;}
        
        if (t-told > 0.5) {freezingButton = FALSE; }
        if (mode == 1) {
            glfemPlotField(theGeometry->theElements,meshSizeField);
            glfemPlotMesh(theGeometry->theElements); 
            sprintf(theMessage, "Number of elements : %d ",theGeometry->theElements->nElem);
            glColor3f(1.0,0.0,0.0); glfemMessage(theMessage); }
        if (mode == 0) {
            domain = domain % theGeometry->nDomains;
            glfemPlotDomain( theGeometry->theDomains[domain]); 
            sprintf(theMessage, "%s : %d ",theGeometry->theDomains[domain]->name,domain);
             glColor3f(1.0,0.0,0.0); glfemMessage(theMessage);  }
            
         glfwSwapBuffers(window);
         glfwPollEvents();
    } while( glfwGetKey(window,GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) != 1 );
            
    // Check if the ESC key was pressed or the window was closed

    free(meshSizeField);
    femElasticityFree(theProblem) ; 
    geoFree();
    glfwTerminate(); 
    
    exit(EXIT_SUCCESS);
    return 0;  
}


 

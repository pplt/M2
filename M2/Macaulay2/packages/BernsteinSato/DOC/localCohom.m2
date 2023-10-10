doc ///
    Key
        [localCohom, Strategy]
    Headline
        specify strategy for local cohomology
    Description
        Text
            There are two main strategies used in the computation of local cohomology: {\tt Walther} and {\tt OaTa}. 
            
            If the user selects {\tt Walther}, which is the default, then @TO localCohom@ uses the algorithm of Walther that relies on the computation of a Čech complex.
            The option @TO LocStrategy@ then determines the localization strategy used for building the Čech complex:
            
            {\tt LocStrategy => null} is used only for {\tt localCohom(...Ideal...)}. Localizations are done by straigthforward computation of annihilators and $b$-functions, as described in [1].
                                
            {\tt LocStrategy => OaTaWa}: localizations are done following the Oaku-Takayama-Walther method [2].
            
            {\tt LocStrategy => Oaku}: localizations are done following Oaku's algorithm.
            
            If the user selects {\tt OaTa}, then @TO localCohom@ uses a restriction from the graph embedding, a method due to T. Oaku and N. Takayama [3]. See @TO Drestriction@.  
    References         
        [1] Walther, {\em Algorithmic computation of local cohomology modules and the local cohomological dimension of algebraic varieties}  (JPAA (139), 1999).
            
        [2] Oaku, Takayama, Walther, {\em A localization algorithm for $D$-modules} (J. Symbolic Computation (29), 2000).
            
        [3] Oaku, Takayama, {\em Algorithms for $D$-modules: restriction, tensor product, localization, and local cohomology groups} (JPAA (156), 2001).
    Caveat 
        {\tt localCohom(...Ideal, Module...)} with the default strategy computes presentations for all the terms in the Čech complex regardless of the requested homological degrees. 
        All strategies use the given generators of the ideal; the user is advised to call @TO mingens@ before calling @TO localCohom@.
    SeeAlso
        LocStrategy
///

--Caveat => {"When OaTaWa strategy is used the error 'Bad luck!' 
--may appear. This means your are not a lucky individual...
--The glitch is due to the fact that the localizations are iterated 
--for this particular strategy; it was resolved for WaltherOaku, 
--a strategy that considers everyone lucky."
--},

doc ///
    Key
        LocStrategy 
        [localCohom, LocStrategy]
    Headline
        specify localization strategy for local cohomology
    Description
        Text
            These strategies determine how presentations of localization in the Čech complex are calculated when selecting Walther's strategy. 
    SeeAlso
        [localCohom, Strategy]
///

doc ///
    Key
        Walther
    Headline
        a valid value for the option Strategy of LocalCohom
    SeeAlso
	[localCohom, Strategy]
///

doc ///
    Key
        OaTa
    Headline
        a valid value for the option Strategy of LocalCohom
    SeeAlso
	[localCohom, Strategy]
///

doc ///
    Key
        OaTaWa
    Headline
        a valid value for the option Strategy of LocalCohom
    SeeAlso
	[localCohom, Strategy]
///

doc ///
    Key
        localCohom
        (localCohom, ZZ, Ideal)
        (localCohom, ZZ, Ideal, Module)
        (localCohom, List, Ideal)
        (localCohom, List, Ideal, Module)
        (localCohom, Ideal)
        (localCohom, Ideal, Module)
    Headline
        compute local cohomology of a holonomic module 
    Usage
        H = localCohom(i, I)
        H = localCohom(i, I, M)
        T = localCohom(L, I)
        T = localCohom(L, I, M)         
        T = localCohom I
        T = localCohom(I, M)
    Inputs
        I:Ideal
            of a polynomial ring $R = \mathbb{Q}[x_1,\ldots,x_n]$
        M:Module
            holonomic over the Weyl algebra $A_n(\mathbb{Q}) = R\langle dx_1, \ldots, dx_n \rangle$
        i:ZZ
        L:List
            containing the desired cohomological degrees 
    Outputs
        H:Module
            the local cohomology module $H^i_I(M)$.
            If the module $M$ is not passed as an argument, then it is assumed to be the polynomial ring $R$.
        T:HashTable
            containing a pair $i \Rightarrow H^i_I(M)$ for each integer $i$ in the list $L$.
            If a list $L$ is not passed as an argument, then $i$ will range over all integers from $0$ to the number of generators of $I$.
            If the holonomic module $M$ is not passed as an argument, then it is assumed to be the polynomial ring $R$.
    Description
        Text
            Given an ideal $I$ of a polynomial ring $R = \mathbb{Q}[x_1,\ldots,x_n]$ and an integer $i$, {\tt localCohom(i, I)} computes the local cohomology module $H^i_I(R)$, returning a presentation for this module over the Weyl algebra $A_n(\mathbb{Q}) = R\langle dx_1,\ldots,dx_n\rangle$.
        Example
            R = QQ[x, y, z];
            I = ideal(x*(y-z), x*y*z);
            localCohom(2, I)
            pruneLocalCohom oo
        Text
            As observed in the example above, the modules returned by {\tt localCohom} are not simplified, and the user is advised to use @TO pruneLocalCohom@ to perform the simplication.
            
            The user may specify a list of the desired cohomological degrees, rather than a single degree.
            The output in this case will be a hash table containing a pair $i \Rightarrow H^i_I(R)$ for each integer $i$ included in the list passed by the user.
        Example
            pruneLocalCohom localCohom({1, 2}, I)
        Text
            If no cohomological degree or list of cohomological degrees is specified, {\tt localCohom} will compute the cohomology modules in all degrees ranging from $0$ to the number of generators of the ideal $I$. 
        Example
            pruneLocalCohom localCohom I
        Text
            The function {\tt localCohom} also computes local cohomology of holonomic modules over the Weyl algebra, and all of the choices for specifying cohomological degrees described above are available in this case as well. 
        Example
            W = QQ[x, dx, y, dy, z, dz, WeylAlgebra => {x => dx, y => dy, z => dz}];
            I = ideal(x*(y-z), x*y*z);        
            pruneLocalCohom localCohom(1, I, W^1/ideal(dx, dy, dz))
            pruneLocalCohom localCohom({1, 2}, I, W^1/ideal(dx, dy, dz))
            pruneLocalCohom localCohom(I, W^1/ideal(dx, dy, dz))
    Caveat
        The modules returned are not simplified; use @TO pruneLocalCohom@ to simplify them.
        All strategies use the given generators of the ideal; the user is advised to call @TO mingens@ before calling @TO localCohom@.
    SeeAlso
        pruneLocalCohom
        [localCohom, Strategy]
        [localCohom, LocStrategy]
///

doc ///
    Key
        pruneLocalCohom
        (pruneLocalCohom, Module)
        (pruneLocalCohom, HashTable)
    Headline
        prunes local cohomology modules
    Usage
        Hi = pruneLocalCohom Hi
        H = pruneLocalCohom H
    Inputs
        Hi:Module
            a local cohomology module returned by @TO localCohom@
        H:HashTable
            a hash table of local cohomology modules, as returned by @TO localCohom@
    Outputs
        Hi:Module
        H:HashTable
    Description
        Text
            This function applies @TO Dprune@ to all the values of {\tt H}. 
    SeeAlso
        localCohom
        Dprune 
///

document {
     Key => {(deRhamAll, RingElement), deRhamAll},
     Headline => "deRham complex for the complement of a hypersurface",
     Usage => "deRhamAll f",
     Inputs => {"f"},
     Outputs => {
	  HashTable => {"containing explicit cohomology classes 
	       in the deRham complex for the complement 
	       of the hypersurface ",  EM "{f = 0}", " and
     	       supplementary information"}	       
	  },
     "The routine deRhamAll can be used to compute cup product structures
     as in the paper 'The cup product structure for complements
     of affine varieties' by Walther(2000).",
     PARA{},
     "For a more basic functionality see ", TO "deRham", ".",
     EXAMPLE lines ///
	R = QQ[x,y]
     	f = x^2-y^3 
     	deRhamAll f
	///,
     SeeAlso => {"deRham", "Dlocalize", "Dintegration"}	
} 

document {
     Key => [deRham,Strategy],
	  "Option is passed to Dresolution. See ",
	  TO [Dresolution,Strategy]
}
document {
     Key => [deRhamAll,Strategy],
	  "Option is passed to Dresolution. See ",
	  TO [Dresolution,Strategy]
}

document {
     Key => {deRham, (deRham, ZZ, RingElement), (deRham, RingElement)},
     Headline => "deRham cohomology groups for the complement of a hypersurface",
     Usage => "M = deRham f, Mi = deRham(i,f)",
     Inputs => {
	  "i" => ZZ,
	  "f" => RingElement
	  },
     Outputs => {
	  "Mi" => Module => {"the i-th deRham cohomology group of the complement 
	  of the hypersurface ",  EM "{f = 0}"},
	  "M" => HashTable => {"containing the entries of the form ", TT "i=>Mi"}  
	  },
     "The algorithm used appears in the paper 'An algorithm for deRham 
     cohomology groups of the complement of an affine variety via D-module
     computation' by Oaku-Takayama(1999).  
     The method is to compute the localization of the polynomial ring 
     by f, then compute the derived integration of the localization.",
     EXAMPLE lines ///
	R = QQ[x,y]
     	f = x^2-y^3 
     	deRham f
	deRham(1,f)
	///,
     SeeAlso => {"deRhamAll", "Dlocalize", "Dintegration"}
     }
document {
     Key => TransferCycles,
     Headline => "a key in the hashtable created by deRham",
     SeeAlso => "deRham"
     }
document {
     Key => CohomologyGroups,
     Headline => "a key in the hashtable created by deRham",
     SeeAlso => "deRham"
     }
document {
     Key => PreCycles,
     Headline => "a key in the hashtable created by deRham",
     SeeAlso => "deRham"
     }
document {
     Key => LocalizeMap,
     Headline => "a key in the hashtable created by deRham",
     SeeAlso => "deRham"
     }
document {
     Key => OmegaRes,
     Headline => "a key in the hashtable created by deRham",
     SeeAlso => "deRham"
     }

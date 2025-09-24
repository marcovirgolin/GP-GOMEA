# Helper: load gpgnomea_rcpp.so lib
.gpg_try_load <- function(path = NULL) {
  syms <- c("gpg_new", "gpg_run", "gpg_predict", "gpg_score",
            "gpg_get_model", "gpg_get_progress_log",
            "gpg_get_n_nodes", "gpg_get_evaluations", "gpg_get_final_population")
  have <- all(vapply(syms, function(s) exists(s, mode = "function"), logical(1)))
  if (have) return(invisible(TRUE))
  if (is.null(path)) {
    stop("Can't find library. Use dyn.load('path_to/gpgomea_rcpp.so') or lib_path constructor parameter.", call. = FALSE)
  }
  dyn.load(path)
  have2 <- all(vapply(syms, function(s) exists(s, mode = "function"), logical(1)))
  if (!have2) stop("Can't find function after dyn.load()")
  invisible(TRUE)
}

# create parameters string
.gpg_build_hparams <- function(params) {
  parts <- character(0)
  for (nm in names(params)) {
    val <- params[[nm]]
    if (nm == "seed" && is.numeric(val) && length(val) == 1 && val < 0) next
    if (nm == "logtofile") {
      # If False, ignore
      if (is.character(val) && length(val) == 1) {
        parts <- c(parts, "--logtofile", shQuote(val))
      }
      next
    }
    # bools: TRUE -> flag without value; FALSE -> ignore
    if (is.logical(val) && length(val) == 1) {
      if (isTRUE(val)) parts <- c(parts, paste0("--", nm))
      next
    }
    # others: --name value
    if (length(val) == 1) {
      parts <- c(parts, paste0("--", nm), as.character(val))
    } else {
      # vector: split with '.'
      parts <- c(parts, paste0("--", nm), paste(val, collapse = ","))
    }
  }
  paste(parts, collapse = " ")
}

# S3 Construtor
gpgomea <- function(
  lib_path = NULL,
  time = 60, generations = -1, evaluations = -1,
  prob = "symbreg", multiobj = FALSE, linearscaling = TRUE, functions = "+_*_-_p/_sqrt_plog", erc = TRUE,
  classweights = FALSE,
  gomea = TRUE, gomfos = "LT",
  subcross = 0.5, submut = 0.5, reproduction = 0.0,
  sblibtype = FALSE, sbrdo = 0.0, sbagx = 0.0,
  unifdepthvar = TRUE, tournament = 4, elitism = 0,
  ims = "5_1", syntuniqinit = 1000, popsize = 500,
  initmaxtreeheight = 4, inittype = FALSE,
  maxtreeheight = 17, maxsize = 1000,
  validation = FALSE,
  coeffmut = FALSE,
  gomcoeffmutstrat = FALSE,
  batchsize = FALSE,
  seed = -1, parallel = 1, caching = FALSE,
  silent = TRUE, logtofile = FALSE
) {
  .gpg_try_load(lib_path)
  params <- list(
    time = time, generations = generations, evaluations = evaluations,
    prob = prob, multiobj = multiobj, linearscaling = linearscaling, functions = functions, erc = erc,
    classweights = classweights,
    gomea = gomea, gomfos = gomfos,
    subcross = subcross, submut = submut, reproduction = reproduction,
    sblibtype = sblibtype, sbrdo = sbrdo, sbagx = sbagx,
    unifdepthvar = unifdepthvar, tournament = tournament, elitism = elitism,
    ims = ims, syntuniqinit = syntuniqinit, popsize = popsize,
    initmaxtreeheight = initmaxtreeheight, inittype = inittype,
    maxtreeheight = maxtreeheight, maxsize = maxsize,
    validation = validation,
    coeffmut = coeffmut,
    gomcoeffmutstrat = gomcoeffmutstrat,
    batchsize = batchsize,
    seed = seed, parallel = parallel, caching = caching,
    silent = silent, logtofile = logtofile
  )

  hp <- .gpg_build_hparams(params)
  handle <- gpg_new(hp)

  model <- list(
    handle = handle,
    params = params,
    lib_path = lib_path
  )
  class(model) <- "gpgomea"
  model
}

print.gpgomea <- function(x, ...) {
  cat("GPGOMEA (wrapper S3)\n")
  cat("Parameters:\n")
  prms <- x$params
  show <- c("prob", "gomea", "ims", "popsize", "time", "generations", "seed", "functions", "linearscaling", "erc", "silent")
  show <- show[show %in% names(prms)]
  for (nm in show) {
    cat(sprintf("  %s: %s\n", nm, paste0(prms[[nm]], collapse = ",")))
  }
  invisible(x)
}

# Fit
fit.gpgomea <- function(object, X, y, ...) {
  if (is.matrix(X)) {
    # ok
  } else if (is.data.frame(X)) {
    X <- as.matrix(X)
  } else {
    stop("X must be matrix or data.frame")
  }
  y <- as.numeric(y)
  if (length(y) != nrow(X)){
    stop("y rows must be equal to X rows")
  }

  gpg_run(object$handle, X, y)
  invisible(object)
}

# Predict
predict.gpgomea <- function(object, X, ...) {
  if (is.matrix(X)) {
    # ok
  } else if (is.data.frame(X)) {
    X <- as.matrix(X)
  } else {
    stop("X must be matrix or data.frame")
  }
  gpg_predict(object$handle, X)
}

# Score
score.gpgomea <- function(object, X, y, ...) {
  if (missing(y)){
   stop("y Missing")
  }
  if (is.data.frame(X)) X <- as.matrix(X)
  y <- as.numeric(y)
  gpg_score(object$handle, X, y)
}

# Modelo string
get_model.gpgomea <- function(object, ...) {
  gpg_get_model(object$handle)
}

# Final Population
get_final_population.gpgomea <- function(object, X, ...) {
  if (is.data.frame(X)) X <- as.matrix(X)
  gpg_get_final_population(object$handle, X)
}

get_n_nodes.gpgomea <- function(object, ...) {
  as.integer(gpg_get_n_nodes(object$handle))
}

get_evaluations.gpgomea <- function(object, ...) {
  as.integer(gpg_get_evaluations(object$handle))
}

get_progress_log.gpgomea <- function(object, ...) {
  gpg_get_progress_log(object$handle)
}

# Like python set_params
set_params.gpgomea <- function(object, ...) {
  updates <- list(...)
  # Only know parameters
  for (nm in names(updates)) {
    object$params[[nm]] <- updates[[nm]]
  }
  # Reopen handle
  hp <- .gpg_build_hparams(object$params)
  if (!is.null(object$lib_path)) .gpg_try_load(object$lib_path)
  object$handle <- gpg_new(hp)
  invisible(object)
}

# Like python get_params
get_params.gpgomea <- function(object, ...) {
  object$params
}

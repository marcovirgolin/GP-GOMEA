set.seed(0)
n <- 300
X <- cbind(rnorm(n), rnorm(n), rnorm(n))
y <- 2.0*X[,1] + 0.5*(X[,2]^2) - 1.5*sin(X[,3]) + rnorm(n, sd=0.1)

make_hyper <- function(seed, gens, pop) {
  sprintf("--seed %d --generations %d --population_size %d --max_tree_size 60 --use_ERC 1 --linear_scaling 1 --silent",
          seed, gens, pop)
}

grid <- expand.grid(seed = c(0,1), generations = c(50, 100), population_size = c(200, 400))
best_score <- -Inf
best <- NULL

for (i in seq_len(nrow(grid))) {
  hyper <- make_hyper(grid$seed[i], grid$generations[i], grid$population_size[i])
  h <- gpg_new(hyper)
  gpg_run(h, X, y)
  sc <- gpg_score(h, X, y)
  if (sc > best_score) {
    best_score <- sc
    best <- list(hyper=hyper, model=gpg_get_model(h))
  }
}

cat("Best score:", best_score, "\n")
cat("Best hyperparameters:", best$hyper, "\n")
cat("Model:", best$model, "\n")